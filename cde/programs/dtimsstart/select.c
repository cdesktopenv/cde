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
/* @(#)$XConsortium: select.c /main/6 1996/11/21 12:17:43 cde-hp $ */

#include	"xims.h"

static int	fill_ims_ent(/* list, top, last */);
static int	setup_local_selection(/* sel, list, conf_all */);
static int	setup_remote_selection(/* sel */);
static int	setup_user_selection(/* sel, list, idx */);


void	ximsSelect(void)
{
    int		ret;
    bool	use_win;
    char	*def_ims;
    ImsList	*list = (ImsList *) 0;
    FileSel	*fsel = (FileSel *) 0;
    UserSelection	*sel = &userSel;

    OpState = State_Select;

    /* clear_UserSelection(sel); */
    sel->iconic = -1;

    if (!localList) {
	if ((ret = get_ims_list(&list, NULL, False)) != NoError) {
	    OpState = State_Select_Err;
	    OpErrCode = ret;
	    return;
	}
	localList = list;
    } else
	list = localList;

    use_win = True;
    def_ims = NULL;

    if (Opt.SelectMode == SEL_MODE_GIVEN) {
	sel->name = NEWSTR(Opt.ImsName);
	use_win = False;
    } else {

	if (read_user_selection(&sel->fsel, NULL) == NoError) {
	    fsel = sel->fsel;
#ifdef	DEBUG
	    if (DebugLvl > 1)	pr_FileSel(sel->fsel);
#endif
	}

	if (list->default_idx >= 0)
	    def_ims = list->elist[list->default_idx]->name;

	if ((OpFlag & FLAG_NOSAVE) || Opt.SelectMode == SEL_MODE_NOAUTO) {
	    use_win = True;
	} else if (Opt.SelectMode == SEL_MODE_AUTO) {
	    use_win = False;
#ifdef	SelectMode_ONCE
	} else if (Opt.SelectMode == SEL_MODE_ONCE) {
	    if (fsel->name)
		use_win = False;
#endif	/* SelectMode_ONCE */

#ifdef	AutoSelectionForSoleIMS
	} else if (list->num_ent == 1) {
	    DPR(("ximsSelect(): only one entry in locale_conf\n"));
	    sel->name = NEWSTR(list->elist[0]->name);
	    use_win = False;
#endif	/* AutoSelectionForSoleIMS */

	} else if (fsel) {
	    if (fsel->select_mode != SEL_MODE_NOAUTO
		&& ((fsel->select_mode == SEL_MODE_AUTO ||
					list->def_selmode == SEL_MODE_AUTO)
			    && (def_ims || fsel->name))
#ifdef	SelectMode_ONCE
		    || ((fsel->select_mode == SEL_MODE_ONCE
			    || list->def_selmode == SEL_MODE_ONCE)
			&& fsel->name)
#endif	/* SelectMode_ONCE */
		    )
		use_win = False;
	} else if (list->def_selmode == SEL_MODE_AUTO && def_ims) {
	    DPR(("ximsSelect(): def_selmode==AUTO in locale_conf\n"));
	    use_win = False;
	}
    }

    if (sel->iconic == -1) {
	sel->iconic = fsel ? fsel->iconic : 0;
    } else if (sel->iconic != fsel->iconic) { /* this should never happen ! */
	sel->flag |= F_SELECT_CHANGED;
    }

    if (fsel && fsel->select_mode == SEL_MODE_NONE) {
	if (list->def_selmode != SEL_MODE_NONE) {
	    fsel->select_mode = list->def_selmode;
	    sel->flag |= F_SELECT_CHANGED;
	}
    }

    if (!sel->name && fsel && fsel->name)
	sel->name = NEWSTR(fsel->name);
    else if (def_ims)
	sel->name = NEWSTR(def_ims);

    if (Opt.HostName)
	sel->hostname = NEWSTR(Opt.HostName);
    else if (fsel && fsel->hostname)
	sel->hostname = NEWSTR(fsel->hostname);

    if (sel->hostname)
	sel->host_type = check_hostname(sel->hostname);
    else
	sel->host_type = HOST_LOCAL;

    if (use_win == True) {
	ret = setup_local_selection(sel, list, True);
	ret = start_selection_window();
    } else {

	switch (sel->host_type) {
	    case HOST_REMOTE:
		ret = setup_remote_selection(sel);
		if (ret != NoError) {
		    put_xims_errmsg(ret, 0, 0, 0);
		}
		break;

	    case HOST_LOCAL:
		ret = setup_local_selection(sel, list, False);
		break;

	    case HOST_UNKNOWN:
		ret = ErrUnknownHost;
		/* put_xims_errmsg(ret, sel->hostname, 0, 0); */
		break;
	}
    }

    OpErrCode = ret;
    OpState = ret == NoError ? State_Select_Done : State_Select_Err;

    return;
}


static int	fill_ims_ent(ImsList *list, int top, int last)
{
    int		ret;
    int		i;
    ImsConf	*ims;
    ImsEnt	*ent;

    /* if (top < 0 || last < 0)	return 0; */
    ims = (ImsConf *) 0;
    for (i = top; i <= last; i++) {	/* read IMS conf */
	ret = NoError;
	ent = list->elist[i];
	if (!ent->ims) {
	    if (!ims)	ims = ALLOC(1, ImsConf);
	    ret = read_imsconf(ims, ent->name, ent->fname);
	    if (ret == NoError) {
		ent->ims = ims;
		ims = (ImsConf *) 0;
	    }
	}
	if (ret == NoError)
	    set_ims_status(ent);
	else
	    ent->status = ret;
    }
    if (ims)	FREE(ims);

    return last - top + 1;
}


static int	setup_local_selection(UserSelection *sel, ImsList *list, int conf_all)
{
    int		ret;
    int		idx, top, last;

    idx = get_ims_idx(list, sel->name);
    top = last = 0;
    if (conf_all)	last = list->num_ent - 1;
    else if (idx >= 0)	top = last = idx;
    fill_ims_ent(list, top, last);

    ret = setup_user_selection(sel, list, idx);

    return ret;
}

static int	setup_remote_selection(UserSelection *sel)
{
    int		ret;
    int		idx;
    ImsList	*new_list;

    ret = get_remote_conf(&new_list, sel->hostname, NULL, sel->name);

    if (ret == NoError) {
	idx = get_ims_idx(new_list, sel->name);
	ret = setup_user_selection(sel, new_list, idx);
    }

    return ret;
}

static int	setup_user_selection(UserSelection *sel, ImsList *list, int idx)
{
    if (sel->name) {
	if (idx >= 0) {
	    sel->ent = list->elist[idx];
	    sel->status = sel->ent->status;
	} else {
	    /* this ims isn't registered in locale_conf */
	    sel->status = ErrNotRegistered;
	}
    } else
	sel->status = ErrNoSelection;
    sel->ims_idx = idx;
    sel->list = list;

    return sel->status;
}

int	update_user_selection(UserSelection *sel, ImsList *list, int idx, char *host, int host_type)
{
    ImsEnt	*ent;

    if (idx < 0 || idx >= list->num_ent)	return False;
    ent = list->elist[idx];

    if (ent->status != NoError)	return False;

    if (strcmp(sel->name, ent->name)) {
	RENEWSTR(sel->name, ent->name);
    }
    if ((host) && (!(sel->hostname) || strcmp(sel->hostname, host))) {
	RENEWSTR(sel->hostname, host);
    }
    sel->host_type = host_type;

    if (sel->list && sel->list != localList && sel->list != list) {
	clear_ImsList(sel->list);
	FREE(sel->list);
    }
    sel->list = list;
    sel->ent = ent;
    sel->status = ent->status;
    sel->ims_idx = idx;
    sel->flag |= F_SELECT_CHANGED;

    return True;
}


int	get_ims_idx(ImsList *list, char *name)
{
    int		idx;

    if (name) {
	for (idx = 0; idx < list->num_ent; idx++)
	    if (strcmp(name, list->elist[idx]->name) == 0)
		return idx;
    }
    return -1;
}

int	set_ims_status(ImsEnt *ent)
{
    int		ret = NoError;
    ImsConf	*ims = ent->ims;

    if (ent->name && strcmp(ent->name, NAME_NONE) == 0) {
	ret = NoError;
    } else if (!ims) {
	ret = ErrNoImsConf;
    } else if (ims->cmd_path && !(ims->flags & F_BUILTIN)
			&& !is_executable(ims->cmd_path)) {
	DPR(("set_ims_status(%s): executable '%s' not exist\n",
					ent->name, ims->cmd_path));
	ret = ErrNoExecutable;
    }

    ent->status = ret;
    return ret;
}


int	get_ims_list(ImsList **listp, char *locale, int fill_ent)
{
    int		ret = NoError;
    ImsList	*list;

    list = ALLOC(1, ImsList);

    if ((ret = read_localeconf(list, locale)) != NoError) {
	ret = ErrNoLocaleConf;
    } else if (list->num_ent == 0) {
	DPR(("get_ims_list(%s): no IMS in locale conf\n", locale));
	ret = ErrNoImsEntry;
    } else if (fill_ent) {
	fill_ims_ent(list, 0, list->num_ent - 1);
    }

    if (ret == NoError) {
	list->status = ret;
    } else {
	clear_ImsList(list);
	FREE(list);
	list = (ImsList *) 0;
    }
    *listp = list;

    return ret;
}

