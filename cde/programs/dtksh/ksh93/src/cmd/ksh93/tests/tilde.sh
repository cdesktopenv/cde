# $XConsortium: tilde.sh /main/3 1995/11/01 16:56:47 rswiston $
function err_exit
{
	print -u2 -n "\t"
	print -u2 -r $Command: "$@"
	let Errors+=1
}

integer Errors=0
Command=$0
OLDPWD=/bin
if	[[ ~ != $HOME ]]
then	err_exit '~' not $HOME
fi
x=~
if	[[ $x != $HOME ]]
then	err_exit x=~ not $HOME
fi
x=x:~
if	[[ $x != x:$HOME ]]
then	err_exit x=x:~ not x:$HOME
fi
if	[[ ~+ != $PWD ]]
then	err_exit '~' not $PWD
fi
x=~+
if	[[ $x != $PWD ]]
then	err_exit x=~+ not $PWD
fi
if	[[ ~- != $OLDPWD ]]
then	err_exit '~' not $PWD
fi
x=~-
if	[[ $x != $OLDPWD ]]
then	err_exit x=~- not $OLDPWD
fi
if	[[ ~root != /  &&  ~root != /root ]]
then	err_exit '~root' not /
fi
x=~root
if	[[ $x != /  &&  $x != /root ]]
then	err_exit 'x=~root' not /
fi
x=~%%
if	[[ $x != '~%%' ]]
then	err_exit 'x='~%%' not '~%%
fi
x=~:~
if	[[ $x != "$HOME:$HOME" ]]
then	err_exit x=~:~ not $HOME:$HOME
fi
exit $((Errors))
