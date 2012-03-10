/* $XConsortium: Environ.C /main/4 1996/04/30 18:12:49 barstow $ */
/*******************************************************************
**  (c) Copyright Hewlett-Packard Company, 1990, 1991, 1992, 1993.
**  All rights are reserved.  Copying or other reproduction of this
**  program except for archival purposes is prohibited without prior
**  written consent of Hewlett-Packard Company.
********************************************************************
****************************<+>*************************************/

#include "Environ.h"

OSEnvironment::OSEnvironment()
{
}


CDEEnvironment::CDEEnvironment
	(
	CString * homedir,
	OSEnvironment * os_
	) : dtspSysApp(0),
	    dtspUserApp(0),
	    dtspSysIcon(0),
	    dtspUserIcon(0),
	    dtspSysHelp(0),
	    dtspUserHelp(0),
	    dtspSysInfoLib(0),
	    dtspUserInfoLib(0),
	    dtspSysDB(0),
	    dtspUserDB(0),
	    dtManPath(0),
	    userHostDir(""),
	    os(os_)
{
    CString envVar;

    sysAdmConfig = "/etc/dt/appconfig";
    factoryInstall = "/usr/dt/appconfig";
    factoryManPath = "/usr/dt/man";

    envVar = os->getEnvironmentVariable("DTSPSYSAPPHOSTS");
    if (!envVar.isNull())
	dtspSysApp = new CString(envVar);

    envVar = os->getEnvironmentVariable("DTSPUSERAPPHOSTS");
    if (!envVar.isNull())
	dtspUserApp = new CString(envVar);

    envVar = os->getEnvironmentVariable("DTSPSYSICON");
    if (!envVar.isNull())
	dtspSysIcon = new CString(envVar);

    envVar = os->getEnvironmentVariable("DTSPUSERICON");
    if (!envVar.isNull())
	dtspUserIcon = new CString(envVar);

    envVar = os->getEnvironmentVariable("DTSPSYSHELP");
    if (!envVar.isNull())
	dtspSysHelp = new CString(envVar);

    envVar = os->getEnvironmentVariable("DTSPUSERHELP");
    if (!envVar.isNull())
	dtspUserHelp = new CString(envVar);

    envVar = os->getEnvironmentVariable("DTSPSYSINFOLIB");
    if (!envVar.isNull())
	dtspSysInfoLib = new CString(envVar);

    envVar = os->getEnvironmentVariable("DTSPUSERINFOLIB");
    if (!envVar.isNull())
	dtspUserInfoLib = new CString(envVar);

    envVar = os->getEnvironmentVariable("DTSPSYSDATABASEHOSTS");
    if (!envVar.isNull())
	dtspSysDB = new CString(envVar);

    envVar = os->getEnvironmentVariable("DTSPUSERDATABASEHOSTS");
    if (!envVar.isNull())
	dtspUserDB = new CString(envVar);

    envVar = os->getEnvironmentVariable("DTMANPATH");
    if (!envVar.isNull())
	dtManPath = new CString(envVar); 

    envVar = os->getEnvironmentVariable("DTUSERSESSION");
    if (!envVar.isNull())
	userHostDir = envVar; 

    if (homedir == 0) {
	envVar = os->getEnvironmentVariable("HOME");
	theHome = envVar;
    }
    else
	theHome = *homedir;
    theHome += "/.dt";

    defaultSearchPath = theHome + "," + sysAdmConfig + "," + factoryInstall;

}


CDEEnvironment::~CDEEnvironment()
{
  delete dtspSysApp;
  delete dtspUserApp;
  delete dtspSysIcon;
  delete dtspUserIcon;
  delete dtspSysHelp;
  delete dtspUserHelp;
  delete dtspSysInfoLib;
  delete dtspUserInfoLib;
  delete dtspSysDB;
  delete dtspUserDB;
  delete dtManPath;

  delete os;
}


void CDEEnvironment::setDTAPPSP
	(
	const CString & sp
	)
{
    delete dtspSysApp;
    dtspSysApp = new CString(sp);
}

void CDEEnvironment::CreateHomeAppconfigDir()
{
    if (!os->FileExists (HOME()))
	os->MakeDirectory (HOME(), 0755);

    if (!os->FileExists (HOME() + "/types"))
	os->MakeDirectory (HOME() + "/types", 0775);

    if (!os->FileExists (HOME() + "/icons"))
	os->MakeDirectory (HOME() + "/icons", 0775);

    if (!os->FileExists (HOME() + "/appmanager"))
	os->MakeDirectory (HOME() + "/appmanager", 0775);

    if (!os->FileExists (HOME() + "/help"))
	os->MakeDirectory (HOME() + "/help", 0775);
}


