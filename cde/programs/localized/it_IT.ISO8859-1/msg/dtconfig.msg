$ $XConsortium: dtconfig.msg /main/5 1996/10/19 16:10:57 rws $
$ *****************************************************************************
$ **
$ **  File:        dtconfig.msg
$ **
$ **  Project:     CDE Session Management (dtconfig)
$ **
$ **  Description:
$ **  -----------
$ **  This file is the source for the message catalog for dtconfig
$ **
$ **  Message set 10: IBM
$ **  Message set 20: Sun
$ **
$ *****************************************************************************

$ *****************************************************************************
$ **  Message set 10: For IBM platforms
$ *****************************************************************************

$set 10
1 è necessario essere superutente. \n
$ 
2 L'ambiente CDE è stato impostato come interfaccia predefinita per l'utente\n\
e verrà utilizzato al login da tutti gli utenti del sistema.\n\
Perché le modifiche abbiano effetto è necessario arrestare e riavviare il sistema.\n
$ 
3 La riga comandi è stata impostata come interfaccia predefinita per l'utente\n\
e verrà utilizzata al login da tutti gli utenti del sistema.\n\
Perché le modifiche abbiano effetto è necessario arrestare e riavviare il sistema.\n
4 L'interfaccia predefinita è già impostata sull'ambiente CDE.\n
$ 
5 L'interfaccia predefinita è già impostata sulla riga comandi.\n
$ 
6 sintassi: %s\n\
  -e (abilita l'avvio automatico di dtlogin)\n\
  -d (disabilita l'avvio automatico di dtlogin)\n\
  -kill (termina dtlogin)\n\
  -reset (ripristina dtlogin - rilegge i file di configurazione)\n
$ 
7 Troppi argomenti \n
$ 
8  È necessario un argomento \n
$ 
9  Impossibile disabilitare xdm, vedere la pagina di spiegazioni relativa\n\
a xdm per informazioni su come disabilitare xdm e riprovare. \n 
$ 
10 Dtlogin non è in funzione \n

$ *****************************************************************************
$ **  Message set 20: For Sun Platforms
$ *****************************************************************************

$set 20
$ 
1  Configurazione del CDE
$ 
2   %s -d     (disabilita l'avvio automatico)
3   %s -e     (abilita l'avvio automatico)
4   %s -kill  (arresta dtlogin)
5   %s -reset (ripristina dtlogin)
6   %s -p     (aggiorna l'azione stampante)
$ 
$	In above messages 2 through 6, only translate words between
$	left "(" and right ").
$ 
7 %s: per eseguire questo script è necessario essere root
8 Sintassi: Troppi argomenti
9 Sintassi: È necessario un argomento
10 opzione non riuscita
11 impossibile eseguire %s
12 disabilitazione non riuscita
13 impossibile eliminare %s
14 fatto
15 l'avvio automatico del CDE è stato disabilitato.
16 abilitazione non riuscita
17 impossibile trovare %s
18 impossibile creare %s
19 impossibile creare %s
20 l'avvio automatico del CDE è stato abilitato.
21 dtlogin terminato.
22 le risorse di configurazione di dtlogin sono state caricate.
23 l'azione stampante è stata aggiornata
24 Sintassi:
$ 
25  %s -inetd    (inetd.conf /usr/dt daemons)
26  %s -inetd.ow (inetd.conf /usr/openwin daemons)
$ 
$	In above messages 25 and 26, only translate words between
$	left "(" and right ").
$ 
27 Al prossimo avvio del sistema, le seguenti utilità verrano esegite da /usr/dt/bin
$ 
28     rpc.ttdbserverd         (ToolTalk)
29     rpc.cmsd                (Gestione dell'Agenda)
30     dtspcd                  (Controllo dei processi secondari)
$ 
$  Keep column allignment on messages 28 thru 30.  
$  Only translate what's beteen ( )'s
$ 
31 Al prossimo avvio del sistema, le seguenti utilità verrano esegite da /usr/openwin/bin
$ 
32     rpc.ttdbserverd         (ToolTalk) 
33     rpc.cmsd                (Gestione dell'Agenda)
$ 
$  Keep column allignment on messages 32 thru 33.  
$  Only translate what's beteen ( )'s
$ 
