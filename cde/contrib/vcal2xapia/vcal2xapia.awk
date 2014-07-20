BEGIN {FS=":"} 
# My attempt at converting .ics files into XAPIA format for dtcm(1)

/^BEGIN/ {if ($2~"VEVENT") {appnt=1; rxtype=0; nxr=1; mxday=0; mxmonth=0; runtil=0}
} 
/^DTSTART/ {sdate=$2} 
/^DTEND/ {fdate=$2} 
/^DESCRIPTION/ {summary=summary" "substr($0,13)}
/^SUMMARY/ {summary=summary" "substr($0,8)}
/^LOCATION/ {summary=summary" in "substr($0,9)} 
/^RRULE/ {rrule=$2;

# The event will repeat forever unless we find a limit 
    nxr=0; 
    rfields=split(rrule,rdata,";");
    for (i=1;i<=rfields;i++) {
	if (rdata[i]~"FREQ") {
	    	rtype=substr(rdata[i],5);
		if (rtype~"DAILY") rxtype=1; 
		if (rtype~"WEEKLY") rxtype=2; 
# We assume a monthly repeat by date for now 
		if (rtype~"MONTHLY") rxtype=5; 
		if (rtype~"YEARLY") rxtype=6;
	}

	if (rdata[i]~"COUNT") {nxr=substr(rdata[i],7)};
	if (rdata[i]~"BYDAY") {
	    rdays=substr(rdata[i],7);
	    nrdays=split(rdata[i],ddays,","); 
# nrdays=1: we have a simple monthly repeat by weekday  
	    if ((nrdays==1) && (rxtype==5)) {rxtype=4};
# with nrdays > 1 we have a problem: 
# XAPIA format only allows limited forms of weekly repeats 
	    if ((nrdays>1) && (rxtype=2)) {
		if (nrdays==2) {
		    tuth=match(ddays[1],"TU")+match(ddays[2],"TH");
		    if (tuth==2) {rxtype=12}; 
		}; 
		if (nrdays==3) { 
		    mowefr=0; 
		    for (j=1;j<=nrdays;j++) {
			mowefr+=match(ddays[j],"MO"); 
			mowefr+=match(ddays[j],"WE");
			mowefr+=match(ddays[j],"FR");
		    };
		    if (mowefr==3) {rxtype=11}; 
		};
		if (nrdays==5) { 
		    wweek=0; 
		    for (j=1;j<=nrdays;j++) {
			wweek+=match(ddays[j],"MO");
			wweek+=match(ddays[j],"TU");
			wweek+=match(ddays[j],"WE");
			wweek+=match(ddays[j],"TH");
			wweek+=match(ddays[j],"FR");
		    };
		    if (wweek==5) {rxtype=10}; 
		};
	    };
	};  
# This is bad: if we have not found a repeat by day matching one XAPIA 
# format, the repetition rule is not fully defined. 
# Most likely we will have only one of the appointments of the week. 
    				      
	     
	    
     
	if (rdata[i]~"BYMONTHDAY=") {mxday=substr(rdata[i],12)}; 
	if (rdata[i]~"BYMONTH=") {mxmonth=substr(rdata[i],9)}; 

	if (rdata[i]~"INTERVAL=") {xinterval=substr(rdata[i],10)
# Weekly appointment with a 2 week interval is definec in XAPIA 
	    if ((xinterval==2) && (rxtype==2)) {rxtype=3} 
# Monthly appointment with a 12 week interval is really a yearly appointment
	    if ((xinterval==2) && (rxtype=5)) {rxtype=6}
# Other cases are not good. csa.h indicates that repeats with 
# an N day/week/month interval are supported by XAPIA
# but it is not clear whether dtcm supports that  
	}; 
# Repetition until a date in the future   
	if (rdata[i]~"UNTIL=") {xuntil=substr(rdata[i],7); runtil=1};   
    }; 
}; 

 

/^END/ {if ($2~"VEVENT") {
# # That one is annoying: we have to calculate the number of repetitions !
# We do that by using an average duration for the year and for the month
# but obviously this is imprecise, and the last appointment may not be there.
# Also, we  must do it here since DTSTART can appear at the very end 
# of an entry. 
	if (runtil==1) { 
	    uyear=substr(xuntil,1,4)-substr(sdate,1,4);
	    umonth=substr(xuntil,5,2)-substr(sdate,5,2);
	    uday=substr(xuntil,7,2)-substr(sdate,7,2); 
	    if (rxtype==6) nxr=uyear+1;
	    if ((rxtype==5)||(rxtype==4)) nxr=12*uyear+umonth+1; 
	    if (rxtype==2) nxr=int((365.25*uyear+30.44*umonth+uday)/7.0)+1; 
	    if (rxtype==3) nxr=int((365.25*uyear+30.44*umonth+uday)/14.0)+1; 
	    if (rxtype==1) nxr=int(365.25*uyear+30.44*umonth+uday)+1;
	    if (rxtype==10) nxr=int(5.0*(365.25*uyear+30.44*umonth+uday)/7.0)+1; 
	    if (rxtype==11) nxr=int(3.0*(365.25*uyear+30.44*umounth+uday)/7.0)+1;
	    if (rxtype==12) nxr=int(3.0*(365.25*uyear+30.44*umounth+uday)/7.0)+1; 
	    if (nxr<0) nxr=0;
	}; 
# Another important thing is that dtcm does not like events that spread 
# over more than 24 hours. So we have to find how many days is fdate-sdate 
# and if that is more than 1 day transform the event into a daily repeat. 
	dyear=substr(fdate,1,4)-substr(sdate,1,4);
	dmonth=substr(fdate,5,2)-substr(sdate,5,2);
	dday=int(substr(fdate,7,2)-substr(sdate,7,2)+30.44*dmonth+365.25*dyear);

	if ((rxtype==0) && (dday>0)) {rxtype=1;nxr=dday+1};  
	

	printf("\n\n")
	print "\t** Calendar Appointment **"
	print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Entry Delimiter//EN:string:begin"; 
	printf("-//XAPIA/CSA/ENTRYATTR//NONSGML Start Date//EN:datetime:%s\n",sdate);
	print "-//XAPIA/CSA/ENTRYATTR//NONSGML Type//EN:uinteger:0";
	print "-//XAPIA/CSA/ENTRYATTR//NONSGML Subtype//EN:string:-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Appointment//EN";
	print "-//XAPIA/CSA/ENTRYATTR//NONSGML Classification//EN:uinteger:0";
	if (rxtype==0) {
	    printf("-//XAPIA/CSA/ENTRYATTR//NONSGML End Date//EN:datetime:%s\n",fdate);
	} else {
	    qdate=substr(sdate,1,8) substr(fdate,9); 
	    printf("-//XAPIA/CSA/ENTRYATTR//NONSGML End Date//EN:datetime:%s\n",qdate);
	}; 
	print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Show Time//EN:sinteger:1";
	printf("-//XAPIA/CSA/ENTRYATTR//NONSGML Summary//EN:string:%s\n",summary);
	print "-//XAPIA/CSA/ENTRYATTR//NONSGML Status//EN:uinteger:2304";
	printf("-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Type//EN:sinteger:%d\n",rxtype);
	printf("-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Times//EN:uinteger:%d\n",nxr);
	print "-//XAPIA/CSA/ENTRYATTR//NONSGML Audio Reminder//EN:reminder:300:";
	print "-//XAPIA/CSA/ENTRYATTR//NONSGML Popup Reminder//EN:reminder:300:"; 
	print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Occurrence Number//EN:sinteger:-1"; 
	print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Interval//EN:uinteger:0"; 
	print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Entry Delimiter//EN:string:end"; 
	printf("\tDate: %s/%s/%s\n",substr(sdate,5,2),substr(sdate,7,2),substr(sdate,1,4));
# The start/end time are in UTC and have to be converted to local time. We assume the local time is UTC+1   
	shour=substr(sdate,10,2)+1;
	smin=substr(sdate,12,2); 
	fhour=substr(fdate,10,2)+1;
	fmin=substr(fdate,12,2); 
	printf("\tStart: %d%s\n",shour,smin) 
	printf("\tEnd: %d%s\n",fhour,fmin)
	if (rxtype==1) {print "\tRepeat: Daily"};
	if (rxtype==2) {print "\tRepeat: Weekly"}; 
	if (rxtype==3) {print "\tRepeat: Every Two Weeks"}; 
	if (rxtype==4) {print "\tRepeat: Monthly By Weekday"}; 
	if (rxtype==5) {print "\tRepeat: Monthly By Date"};
	if (rxtype==10) {print "\tRepeat: Monday thru Friday"};
	if (rxtype==11) {print "\tRepeat: Mon, Wed, Fri"}; 
	if (rxtype==12) {print "\tRepeat: Tuesday, Thursday"}; 
	printf("\tFor: %d\n",nxr);
	printf("\tWhat: %s\n",summary);
	printf("\t\n"); 
	    
	sdate="";
	fdate="";
	appnt=0;
	summary="";
    } 
 
}