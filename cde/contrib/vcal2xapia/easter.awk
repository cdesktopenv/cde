
BEGIN { 
    y=2016; # change to the appropriate year ! 
    lang=ENVIRON["LANG"] 
    easter["C"]="Easter"
    easter["fr_FR.ISO8859-1"]="P?ques" 
    easter["it_IT.ISO8859-1"]="Pasqua"
    easter["es_ES.ISO8859-1"]="Pascua"
    easter["de_DE.ISO8859-1"]="Ostersonntag" 

    ascension["C"]="Ascension Day" 
    ascension["fr_FR.ISO8859-1"]="Ascension" 
    ascension["it_IT.ISO8859-1"]="Ascensione"
    ascension["es_ES.ISO8859-1"]="Ascension" 
    ascension["de_DE.ISO8859-1"]="Christi Himmelfahrt" 

    whit["C"]="Whit Sunday" 
    whit["fr_FR.ISO8859-1"]="Pentec?te" 
    whit["it_IT.ISO8859-1"]="Spirito Sancto"
    whit["es_ES.ISO8859-1"]="Pentecost?s" 
    whit["de_DE.ISO8859-1"]="Pfingstsonntag"

# Formula to calculate the date of Easter 
    g=y%19;
    c=int(y/100); 
    c4=int(c/4);
    e=int((8*c+13)/25);
    h=(19*g+c-c4-e+15)%30;
    k=int(h/28); 
    p=int(29/(h+1));
    q=int((21-g)/11); 
    i=(k*p*q-1)*k+h;
    b=int(y/4)+y; 
    j1=b+i+2+c4-c; 
    j2=j1%7; 
    r=28+i-j2;
    m=int(r/32)+3;
    d=r-31*(m-3); 

# Creating an appointment in XAPIA format for Easter 
    sdate=sprintf("%.4d%.2d%.2dT004100Z",d,m,y);
    fdate=sprintf("%.4d%.2d%.2dT004200Z",d,m,y);
    appt="easter_"y; 
    printf("\n\n") >appt 
    print "\t** Calendar Appointment **" >appt
        print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Entry Delimiter//EN:string:begin">appt; 
        printf("-//XAPIA/CSA/ENTRYATTR//NONSGML Start Date//EN:datetime:%s\n",sdate)>appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Type//EN:uinteger:0">appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Subtype//EN:string:-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Appointment//EN">appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Classification//EN:uinteger:0">appt;
	printf("-//XAPIA/CSA/ENTRYATTR//NONSGML End Date//EN:datetime:%s\n",fdate)>appt;
	print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Show Time//EN:sinteger:0">appt;
        printf("-//XAPIA/CSA/ENTRYATTR//NONSGML Summary//EN:string: %s %.4d\n",easter[lang],y)>appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Status//EN:uinteger:2304">appt;
        printf("-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Type//EN:sinteger:0\n")>appt;
        printf("-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Times//EN:uinteger:0\n")>appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Audio Reminder//EN:reminder:300:">appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Popup Reminder//EN:reminder:300:">appt; 
        print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Occurrence Number//EN:sinteger:-1">appt; 
        print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Interval//EN:uinteger:0">appt; 
        print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Entry Delimiter//EN:string:end">appt; 
        printf("\tDate: %s/%s/%s\n",m,d,y)>appt;         
        printf("\tStart: 0241\n",shour,smin)>appt 
        printf("\tEnd: 0242\n",fhour,fmin)>appt
	printf("\tRepeat: One Time\n")>appt; 
	printf("\tFor: 0\n")>appt;
        printf("\tWhat: %s %.4d\n",easter[lang],y)>appt;
        printf("\t")>appt;

# Ascension day is 40 days after easter
 
	aday=r+39;
#	print aday; # for debugging purpose only !  
	m=3;
	if (aday>31) {aday-=31;m++ }
	if (aday>30) {aday-=30;m++ } 
	if (aday>31) {aday-=31;m++} 
	if (aday>30) {aday-=30;m++} 
	
	sdate=sprintf("%.4d%.2d%.2dT004100Z",aday,m,y);
	fdate=sprintf("%.4d%.2d%.2dT004200Z",aday,m,y);
	appt="ascension_"y; 

	printf("\n\n") >appt 
    print "\t** Calendar Appointment **" >appt
        print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Entry Delimiter//EN:string:begin">appt; 
        printf("-//XAPIA/CSA/ENTRYATTR//NONSGML Start Date//EN:datetime:%s\n",sdate)>appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Type//EN:uinteger:0">appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Subtype//EN:string:-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Appointment//EN">appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Classification//EN:uinteger:0">appt;
	printf("-//XAPIA/CSA/ENTRYATTR//NONSGML End Date//EN:datetime:%s\n",fdate)>appt;
	print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Show Time//EN:sinteger:0">appt;
        printf("-//XAPIA/CSA/ENTRYATTR//NONSGML Summary//EN:string: %s %.4d\n",ascension[lang],y)>appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Status//EN:uinteger:2304">appt;
        printf("-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Type//EN:sinteger:0\n")>appt;
        printf("-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Times//EN:uinteger:0\n")>appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Audio Reminder//EN:reminder:300:">appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Popup Reminder//EN:reminder:300:">appt; 
        print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Occurrence Number//EN:sinteger:-1">appt; 
        print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Interval//EN:uinteger:0">appt; 
        print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Entry Delimiter//EN:string:end">appt; 
        printf("\tDate: %s/%s/%s\n",m,aday,y)>appt;         
        printf("\tStart: 0241\n",shour,smin)>appt 
        printf("\tEnd: 0242\n",fhour,fmin)>appt
	printf("\tRepeat: One Time\n")>appt; 
	printf("\tFor: 0\n")>appt;
        printf("\tWhat: %s %.4d\n",ascension[lang],y)>appt;
        printf("\t")>appt;

# Whit Monday is 50 days after easter
	wday=r+48;
	m=3; 
	if (wday>31) {wday-=30;m++}
	if (wday>30) {wday-=30;m++} 
	if (wday>31) {wday-=31;m++} 
	if (wday>30) {wday-=30;m++} 

	sdate=sprintf("%.4d%.2d%.2dT004100Z",wday,m,y);
	fdate=sprintf("%.4d%.2d%.2dT004200Z",wday,m,y);
	appt="whit_"y; 

	

	printf("\n\n") >appt 
    print "\t** Calendar Appointment **" >appt
        print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Entry Delimiter//EN:string:begin">appt; 
        printf("-//XAPIA/CSA/ENTRYATTR//NONSGML Start Date//EN:datetime:%s\n",sdate)>appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Type//EN:uinteger:0">appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Subtype//EN:string:-//XAPIA/CSA/SUBTYPE//NONSGML Subtype Appointment//EN">appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Classification//EN:uinteger:0">appt;
	printf("-//XAPIA/CSA/ENTRYATTR//NONSGML End Date//EN:datetime:%s\n",fdate)>appt;
	print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Show Time//EN:sinteger:0">appt;
        printf("-//XAPIA/CSA/ENTRYATTR//NONSGML Summary//EN:string: %s %.4d\n",whit[lang],y)>appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Status//EN:uinteger:2304">appt;
        printf("-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Type//EN:sinteger:0\n")>appt;
        printf("-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Times//EN:uinteger:0\n")>appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Audio Reminder//EN:reminder:300:">appt;
        print "-//XAPIA/CSA/ENTRYATTR//NONSGML Popup Reminder//EN:reminder:300:">appt; 
        print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Occurrence Number//EN:sinteger:-1">appt; 
        print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Repeat Interval//EN:uinteger:0">appt; 
        print "-//CDE_XAPIA_PRIVATE/CSA/ENTRYATTR//NONSGML Entry Delimiter//EN:string:end">appt; 
        printf("\tDate: %s/%s/%s\n",m,wday,y)>appt;         
        printf("\tStart: 0241\n",shour,smin)>appt 
        printf("\tEnd: 0242\n",fhour,fmin)>appt
	printf("\tRepeat: One Time\n")>appt; 
	printf("\tFor: 0\n")>appt;
        printf("\tWhat: %s %.4d\n",whit[lang],y)>appt;
        printf("\t")>appt;

}

# We can also calculate the date of Pentecost (Whit Sunday/Monday) = Easter +50 days 
# and of Ascension Day = Easter + 4O days 
# To write to files in awk, it is enough to do print(f) something > FILE 
