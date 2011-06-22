/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef JSCSSStyleSheet_h
#define JSCSSStyleSheet_h

#include "JSStyleSheet.h"

namespace WebCore {

class CSSStyleSheet;

class JSCSSStyleSheet : public JSStyleSheet {
    typedef JSStyleSheet Base;
public:
    JSCSSStyleSheet(KJS::JSObject* prototype, CSSStyleSheet*);
    virtual bool getOwnPropertySlot(KJS::ExecState*, const KJS::Identifier& propertyName, KJS::PropertySlot&);
    KJS::JSValue* getValueProperty(KJS::ExecState*, int token) const;
    virtual const KJS::ClassInfo* classInfo() const { return &s_info; }
    static const KJS::ClassInfo s_info;

    enum {
        // Attributes
        OwnerRuleAttrNum, CssRulesAttrNum, RulesAttrNum, 
    };
};


class JSCSSStyleSheetPrototype : public KJS::JSObject {
public:
    static KJS::JSObject* self(KJS::ExecState*);
    virtual const KJS::ClassInfo* classInfo() const { return &s_info; }
    static const KJS::ClassInfo s_info;
    bool getOwnPropertySlot(KJS::ExecState*, const KJS::Identifier&, KJS::PropertySlot&);
    JSCSSStyleSheetPrototype(KJS::ExecState* exec)
        : KJS::JSObject(JSStyleSheetPrototype::self(exec)) { }
};

// Functions

KJS::JSValue* jsCSSStyleSheetPrototypeFunctionInsertRule(KJS::ExecState*, KJS::JSObject*, KJS::JSValue*, const KJS::ArgList&);
KJS::JSValue* jsCSSStyleSheetPrototypeFunctionDeleteRule(KJS::ExecState*, KJS::JSObject*, KJS::JSValue*, const KJS::ArgList&);
KJS::JSValue* jsCSSStyleSheetPrototypeFunctionAddRule(KJS::ExecState*, KJS::JSObject*, KJS::JSValue*, const KJS::ArgList&);
KJS::JSValue* jsCSSStyleSheetPrototypeFunctionRemoveRule(KJS::ExecState*, KJS::JSObject*, KJS::JSValue*, const KJS::ArgList&);
} // namespace WebCore

#endif