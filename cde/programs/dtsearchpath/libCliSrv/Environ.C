/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
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


