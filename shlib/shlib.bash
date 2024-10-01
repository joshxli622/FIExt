#! /bin/bash

## 
## Bash Function Library
##
## Author: Sebastian Hauer
##

# Rational for this crazy conditional variable assignment logic:
# 
# MAIL_SENDER and MAIL_RCPT is to be defined by the calling shell script in
# order to define script specific email addresses different from the default.
# 
# MAIL_SENDER_OVERRIDE and MAIL_RCPT_OVERRIDE is meant to be used as an
# override facility from the command line.
# 
# If neither MAIL_SENDER, MAIL_RCPT, MAIL_SENDER_OVERRIDE and 
# MAIL_RCPT_OVERRIDE are set SHLIB_MAIL_SENDER will default
# to `whoami`@`hostname`.joshxli-non-exit.com and SHLIB_MAIL_RCPT
# will default to api-support@joshxli-non-exit.com.

## Init platform dependant commands
case `/usr/bin/uname` in
    SunOS*)
        NOHUP=/usr/xpg4/bin/nohup
        WHOAMI=/usr/ucb/whoami
        ;;
    *)
        NOHUP=nohup
        WHOAMI=whoami
        ;;
esac


SHLIB_MAIL_SENDER=${MAIL_SENDER_OVERRIDE:-${MAIL_SENDER:-"`${WHOAMI}`@`/usr/bin/hostname`.joshxli-non-exit.com"}}
SHLIB_MAIL_RCPT=${MAIL_RCPT_OVERRIDE:-${MAIL_RCPT:-"api-support@joshxli-non-exit.com"}}

email () {
    subj="$1"
    msg="$2"
    rcpt=${3:-${SHLIB_MAIL_RCPT}}
    sndr=${4:-${SHLIB_MAIL_SENDER}}
    
    oifs=$IFS
    rcpts=""
    IFS=",;"
    for r in $rcpt ; do
	    rcpts="${rcpts} $r"
    done
    IFS=$oifs
    echo -e "${msg}" | /usr/bin/mailx -r "${sndr}" -s "${subj}" "${rcpts}"
}

email_tail () {
    file="$1"
    subj="$2"
    msg="$3"
    rcpt=${4:-${SHLIB_MAIL_RCPT}}
    sndr=${5:-${SHLIB_MAIL_SENDER}}
    
    tailout=`/usr/bin/tail -30 $file`
    fullmsg=`echo -e "${msg}\n\n${tailout}"`
    oifs=$IFS
    rcpts=""
    IFS=",;"
    for r in $rcpt ; do
	    rcpts="${rcpts} ${r}"
        echo "r: ${r}"
    done
    IFS=$oifs
    /usr/bin/mailx -r "${sndr}" -s "${subj}" "${rcpts}" <<EOF
${fullmsg}
EOF
}

cat_strs () {
    msg=""
    for m in "$@"; do
        msg="${msg}${m}\n"
    done
    echo $msg
}

extr_subj () {
    msg="$1"
    echo -e -n $msg | /usr/bin/head -1 | cut -b1-48
}

now_dtm () {
    /usr/bin/date +'%Y-%m-%d %H:%M:%S'
}

log () {
    scat=$1
    lcat=$2
    shift
    shift
    msg=`cat_strs "$@"`
    subj=`extr_subj "$msg"`
    subj="${scat}:[${APP_NAME}] ${subj}"
    now=`now_dtm`
    [[ -n "$TERM" && "$TERM" != "dumb" ]] && echo -e -n "${now} [${lcat}]: $msg" >&2
    email "${subj}" "${msg}"
}

log_ftl () {
    log "F" "FATAL" "$*"
}
log_err () {
    log "E" "ERROR" "$*"
}

log_wrn () {
    log "W" "WARN" "$*"
}

log_inf () {
    log "I" "INFO" "$*"
}

init_sys_env () {
    if [ -f ~/sys_root.sh ]; then
        . ~/sys_root.sh
    else
        log_ftl "~/sys_root.sh not found!"
        exit 1
    fi
    if [ -z "$SYS_ROOT" ]; then
        log_ftl "sys_root failed to locate sys_env for this OS and architecture!"
        exit 2
    fi
    if [ -z "$LIB_DIR" ]; then
        log_ftl "You forgot to define LIB_DIR"
        exit 2
    fi
    if [ -n "$SHLIB_DIR" ]; then
        app_dir=$(cd "${SHLIB_DIR}/.." && pwd)
        app_env="${app_dir}.env"
        if [ -f "${app_env}" ]; then
            . "${app_env}"
        fi
    fi
    
    LD_LIBRARY_PATH_64=${LIB_DIR}:${LD_LIBRARY_PATH_64}
    export LD_LIBRARY_PATH_64
}

sim_launch_usage () {
    caller=$1
    [ -z "$TERM" ] && log_err "Invalid launch command type: $1" || {
        cat<<EOF
Usage: ${caller} {start|run|stop}

EOF
    }
    exit 3
}

source_profile () {
    profile=$1
    profile_env="${ETC_DIR}/${profile}.env"
    if [ ! -f "$profile_env" ]; then
        log_ftl "Failed to locate launch profile named: ${profile} in ${profile_env}"
        exit 3
    fi
    . ${profile_env}
    if [ ! -x "${LAUNCH_APP}" ]; then
        log_ftl "Failed to locate executable ${LAUNCH_APP}!"
        exit 3
    fi
    if [ ! -d "$LAUNCH_LOG_DIR" ]; then
        log_ftl "Invalid log dir specified in ${profile_env}, log dir: $LAUNCH_LOG_DIR"
        exit 3
    fi
    [ -z "${LAUNCH_APP}" ] && log_ftl "No LAUNCH_APP was defined in ${profile_env}!"
    export ETC_DIR BIN_DIR LAUNCH_LOG_DIR
}

start_app () {
    source_profile "$1"
    cd "${BIN_DIR}/.."
    today=`/usr/bin/date +'%Y%m%d'`
    log_file="${LAUNCH_LOG_DIR}/${APP_NAME:-${LAUNCH_APP}}-${today}.log"
    cmd="${NOHUP} ${LAUNCH_APP} ${LAUNCH_ARGS} >> ${log_file} 2>&1 &"
    echo "`now_dtm`: ${cmd}" >> ${log_file}
    eval "${cmd}"
}

run_app () {
    source_profile "$1"
    cd "${BIN_DIR}/.."
    cmd="${LAUNCH_APP} ${LAUNCH_ARGS}"
    echo "`now_dtm`: ${cmd}"
    eval "${cmd}"
    echo "`now_dtm`: [done]"
}

stop_app () {
    source_profile "$1"
    echo "`now_dtm`: Not implemented yet" >&2
}

