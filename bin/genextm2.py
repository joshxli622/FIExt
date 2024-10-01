#!/usr/bin/denv python

import sys
import os.path as p
from optparse import OptionParser
import smtplib
import getpass
import traceback
import cStringIO
import datetime
import math
import pytz
from sets import ImmutableSet

bin_dir = p.dirname(p.abspath(sys.argv[0]))
pirate_tools_dir = p.dirname(bin_dir)
pirate_tools_py_dir = p.join(pirate_tools_dir, 'py')
sys.path.insert(0, pirate_tools_py_dir)

from pirate.order import read_orders, Order, OCO, OTO, STP, LMT, MRKT, MOC, MOO
from pirate.data.ibrefdata import load_ref_data
from pirate.data.ibpofdata import load_pof_data
from pirate.util import tzconv


parser = OptionParser(description = 'Generating exit timing',
                      version = '$VERSION:3.12:VERSION$',
                      usage = 'genextm.py -p portfolio file -o ORDTXT')
parser.add_option('-i', '--input', dest = 'input', help = 'input signal file')
parser.add_option('-o', '--output', dest = 'output', help = 'exit timing file.')
parser.add_option('-p', '--pof-data', dest = 'pof_data', help = 'Portfolio data file.')
parser.add_option('-r', '--ref-data', dest = 'ref_data', help = 'Reference data file.')
parser.add_option('-s', '--strat', dest = 'strat', help = 'Strategy name')
parser.add_option('--execute-today', dest = 'exec_today', default=False, action="store_true",
                  help = 'force excute today rather than tomorrow which is default')

(opts, args) = parser.parse_args()

if not (opts.input and opts.output and opts.pof_data and opts.ref_data):
    parser.print_help()
    print
    raise SystemExit('Wrong number of arguments')

def trade_times(run_date_time, execToday,
                trd_end_exchtm, 
                enEndAdj, exPeriod,
                exch_tz, signal_tz = 'America/New_York'):
    """ compute trade date and time from signal date 
    Args: (all arguments are strings)
    signal_date -- signal date string in YYYYMMDD format. 
    trd_end_exchtm -- end time in format HH:MM
    exch_tz -- exchange timezone
    signal_tx -- signal timezone default to America/New_York

    logic for trade date calculation
    As we have no information of accurate trading calendar, we need to be 
    smart to calculate trade date from signal date. 

    Suppose market trade on 24 26 27 (all are weekdays).  If our script is 
    run on 24 and 25, on both days, the signal date from order file will be 24 
    (last available data point). In 24, we will put an order with trade date 
    25. The order will fail on 25 as market closes. Then, on 25, we generate 
    another order with signal date is still 24, but the trade date will be 26. 
    on 26, this order is executed successfully. 

    Therefore, trade date is not only a function of signal date. On 24 and 25, we 
    have the same signal date in order file. But we need to generate different 
    trade date. The trade date should depend on both signal date (which is the 
    last available date date) and current date when the script is run. The trade 
    date should be the next date of the later of the signal date and script run date. 

    Another complication comes in if we run our script after midnight. In this case, 
    the script run date will be moved to the next date already. For example, if we 
    run the script at 1:00am on 25. The trade date should still be 25 rather than 26. 

    If we run script before data update time (6:00pm now), 
    trade date = next weekday >= max(signal date, script run date)
    (e.g. run at 1:00am on 25. The trade date will be 25.) 
    
    If we run script after data update time (6:00pm now), 
    trade date = next weekday > max(signal date, script run date. 
    (e.g. if we run at 7:00pm on 24, trade date will be 25
          if we run at 7:00pm on 25, trade date will be 26. )

    However, signal date is always before script run date. Therefore, what only matters
    is script run date. 

    If --exec_today is supplied on command line, we always execute today
    otherwise, use run_date_time to make the best guess. 

    """

    run_date = run_date_time.date()

    trddt = None
    if execToday or run_date_time.hour <= 18:
        trddt = run_date
    else:
        trddt = run_date + datetime.timedelta(days=1)

    if trddt.weekday() > 4:
        trddt = trddt + datetime.timedelta(days=7-trddt.weekday()) 
    [trd_end_hr, trd_end_min] = [int(i) for i in trd_end_exchtm.split(":")]

    exch_tz_info = pytz.timezone(exch_tz)

    trd_end_dtm = exch_tz_info.localize(
        datetime.datetime(year=trddt.year, month=trddt.month,
                          day=trddt.day, hour=trd_end_hr,
                          minute=trd_end_min, second=0,
                          microsecond=0))

# the following does not work correctly with dst time. We need to use localize method as above
    '''
    trd_end_dtm = datetime.datetime(year=trddt.year, month=trddt.month,
                                    day=trddt.day, hour=trd_end_hr,
                                    minute=trd_end_min, second=0,
                                    microsecond=0, tzinfo=exch_tz_info)
    '''

    trd_end_dtm_ny = tzconv.convert(trd_end_dtm, 'America/New_York')

    enter_end_ny = trd_end_dtm_ny - datetime.timedelta(minutes=enEndAdj)
    exit_begin_ny = trd_end_dtm_ny - datetime.timedelta(minutes=exPeriod)
    exit_end_ny = trd_end_dtm_ny
                                               
    return (tzconv.dtmToString(enter_end_ny),
            tzconv.dtmToString(exit_begin_ny),
            tzconv.dtmToString(exit_end_ny))



error_trace = None
print opts.pof_data
try:    
    mktList = []
    with open(opts.input, 'r') as f:
        dialect = csv.Sniffer().sniff(csvfile.read(1024))
        csvfile.seek(0)
        dr = csv.DictReader(csvfile, dialect)
        for row in dr:
            if row['Type'].strip() == 'MOC':
                mktList.append(int(row['MktID'].strip()))

    rd_fname = opts.ref_data
    rd = load_ref_data(rd_fname)
    pd_fname = opts.pof_data
    pd = load_pof_data(pd_fname)

    (pofDir, pofFile) = p.split(pd_fname)
    (pofFileName, pofFileExt) = p.splitext(pofFile)
    runDtStr = pofFileName.split("_")[2]
    rundt = datetime.datetime.strptime(runDtStr, '%Y-%m-%d')

    outf = sys.stdout if opts.output == '-' else open(opts.output, 'w')
    outs = cStringIO.StringIO()

    title_row = '\t'.join(
        ['Strat', 'MktID', 'IBRoot', 'IBExch', 
         'EnEndNY', 'ExOT', 'ExBeginNY', 'ExEndNY',
         'BB', 'Ric', 'Venue', 'ExchTz', 'MarketName']) 

    print >> outs, title_row

    fmt = ('%(Strat)s\t%(MktID)s\t%(IBRoot)s\t%(IBExch)s\t' +
           '%(EnEndNY)s\t%(ExOT)s\t%(ExBeginNY)s\t%(ExEndNY)s\t' + 
           '%(BB)s\t%(Ric)s\t%(Venue)s\t%(ExchTz)s\t%(MarketName)s')
    
    for mkt_id in pd.mkts:
        pfrow = pd[mkt_id]
        if pfrow.trade != 'Y': continue
        if not int(mkt_id) in mktList: continue 

        rfrow = rd[int(mkt_id)]
        (enter_end_ny, exit_begin_ny, exit_end_ny) = trade_times(
            rundt, opts.exec_today, pfrow.close_tm, 
            int(pfrow.en_end_adj), int(pfrow.ex_period), rfrow.exch_tz)  
    
        print >> outs, fmt % {
            'Strat':opts.strat,
            'MktID':rfrow.mkt_id,
            'IBRoot':rfrow.ib_root,
            'IBExch':rfrow.ib_exch,

            'EnEndNY':enter_end_ny,
            'ExOT':pfrow.ex_ot,
            'ExBeginNY':exit_begin_ny,
            'ExEndNY':exit_end_ny,

            'BB':rfrow.sym_bb,
            'Ric':rfrow.sym_ric,
            'Venue':rfrow.venue,
            'ExchTz':rfrow.exch_tz,
            'MarketName':rfrow.mkt_name}
    
    extms = outs.getvalue()
    outs.close()
    print >> outf, extms
except:
    error_trace = traceback.format_exc()
    print >>sys.stderr, '\nFailed to generate orders: %s' % error_trace

if error_trace:
    sys.exit(1)
else:
    sys.exit(0)
     
