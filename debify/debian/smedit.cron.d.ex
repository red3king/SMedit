#
# Regular cron jobs for the smedit package
#
0 4	* * *	root	[ -x /usr/bin/smedit_maintenance ] && /usr/bin/smedit_maintenance
