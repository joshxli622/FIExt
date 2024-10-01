#! /bin/bash

# Define APP_NAME for shlib.bash's log functions
APP_NAME=`basename $0 | cut -d. -f1`

# Define BIN_DIR, ETC_DIR for shlib.bash's functions.
BIN_DIR=$(cd `dirname $0` && pwd)
LIB_DIR=$(cd "${BIN_DIR}/../lib" && pwd)
ETC_DIR=$(cd "${BIN_DIR}/../etc" && pwd)
SHLIB_DIR=$(cd "${BIN_DIR}/../shlib" && pwd)
shlib=${SHLIB_DIR}/shlib.bash

. "${shlib}"

init_sys_env

profile="${2:-${LAUNCH_PROFILE_OVERRIDE:-RIntra-sim}}"
case "$1" in
    start)
        start_app "$profile"
        ;;
    run)
        run_app "$profile"
        ;;
    stop)
        stop_app "$profile"
        ;;
    env)
        /usr/bin/env
        ;;
    *)
        sim_launch_usage "$0"
        ;;
esac
