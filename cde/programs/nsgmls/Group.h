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
/* $XConsortium: Group.h /main/1 1996/07/29 16:52:34 cde-hp $ */
// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef Group_INCLUDED
#define Group_INCLUDED 1
#ifdef __GNUG__
#pragma interface
#endif

#include "Boolean.h"
#include "ContentToken.h"
#include "StringC.h"
#include "MessageArg.h"
#include "Owner.h"
#include "Syntax.h"
#include "Text.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class GroupToken {
public:
  GroupToken() : type(invalid) { }
  enum Type {
    invalid,
    nameToken,
    name,
    dataTagLiteral,		// data tag (padding) template
    dataTagGroup,
    elementToken,
    modelGroup,
    pcdata,
    dataTagTemplateGroup
    };
  Type type;
  StringC token;		// name nameToken; with substitution
  Owner<ModelGroup> model;
  Owner<ContentToken> contentToken; // elementToken pcdata dataTagGroup
  Text text;
  Vector<Text> textVector;
private:
  GroupToken(const GroupToken &); // undefined
  void operator=(const GroupToken &); // undefined
};

class AllowedGroupTokens {
public:
  AllowedGroupTokens(GroupToken::Type,
		     GroupToken::Type = GroupToken::invalid,
		     GroupToken::Type = GroupToken::invalid,
		     GroupToken::Type = GroupToken::invalid);
  Boolean groupToken(GroupToken::Type) const;
  // modelGroup, dataTagTemplateGroup
  GroupToken::Type group() const;
  GroupToken::Type nameStart() const;
private:
  void allow(GroupToken::Type);
  unsigned flags_;
};

struct GroupConnector {
  enum Type {
    andGC,
    orGC,
    seqGC,
    grpcGC,
    dtgcGC
  };
  Type type;
};

class AllowedGroupConnectors {
public:
  AllowedGroupConnectors(GroupConnector::Type);
  AllowedGroupConnectors(GroupConnector::Type, GroupConnector::Type);
  AllowedGroupConnectors(GroupConnector::Type, GroupConnector::Type,
			 GroupConnector::Type);
  AllowedGroupConnectors(GroupConnector::Type, GroupConnector::Type,
			 GroupConnector::Type, GroupConnector::Type);
  Boolean groupConnector(GroupConnector::Type) const;
private:
  void allow(GroupConnector::Type);
  unsigned flags_;
};

class AllowedGroupTokensMessageArg : public MessageArg {
public:
  AllowedGroupTokensMessageArg(const AllowedGroupTokens &allow,
			       const ConstPtr<Syntax> &syntax);
  MessageArg *copy() const;
  void append(MessageBuilder &) const;
private:
  AllowedGroupTokens allow_;
  ConstPtr<Syntax> syntax_;
};

class AllowedGroupConnectorsMessageArg : public MessageArg {
public:
  AllowedGroupConnectorsMessageArg(const AllowedGroupConnectors &allow,
				   const ConstPtr<Syntax> &syntax);
  MessageArg *copy() const;
  void append(MessageBuilder &) const;
private:
  AllowedGroupConnectors allow_;
  ConstPtr<Syntax> syntax_;
};

inline
Boolean AllowedGroupTokens::groupToken(GroupToken::Type i) const
{
  return ((1 << i) & flags_) != 0;
}

inline
GroupToken::Type AllowedGroupTokens::group() const
{
  if (groupToken(GroupToken::modelGroup))
    return GroupToken::modelGroup;
  else if (groupToken(GroupToken::dataTagTemplateGroup))
    return GroupToken::dataTagTemplateGroup;
  else
    return GroupToken::invalid;
}

inline
GroupToken::Type AllowedGroupTokens::nameStart() const
{
  if (groupToken(GroupToken::elementToken))
    return GroupToken::elementToken;
  else if (groupToken(GroupToken::nameToken))
    return GroupToken::nameToken;
  else if (groupToken(GroupToken::name))
    return GroupToken::name;
  else
    return GroupToken::invalid;
}

inline
void AllowedGroupTokens::allow(GroupToken::Type t)
{
  flags_ |= (1 << t);
}


inline
Boolean AllowedGroupConnectors::groupConnector(GroupConnector::Type c) const
{
  return (flags_ & (1 << c)) != 0;
}

inline
void AllowedGroupConnectors::allow(GroupConnector::Type c)
{
  flags_ |= (1 << c);
}


#ifdef SP_NAMESPACE
}
#endif

#endif /* not Group_INCLUDED */
