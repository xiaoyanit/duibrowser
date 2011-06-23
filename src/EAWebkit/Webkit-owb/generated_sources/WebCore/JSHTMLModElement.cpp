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

/*
* This file was modified by Electronic Arts Inc Copyright � 2009-2010
*/

#include "config.h"

#include "JSHTMLModElement.h"

#include <wtf/GetPtr.h>

#include "HTMLModElement.h"
#include "KURL.h"

using namespace KJS;

namespace WebCore {

/* Hash table */

static const HashTableValue JSHTMLModElementTableValues[4] =
{
    { "cite", (intptr_t)JSHTMLModElement::CiteAttrNum, DontDelete, 0 },
    { "dateTime", (intptr_t)JSHTMLModElement::DateTimeAttrNum, DontDelete, 0 },
    { "constructor", (intptr_t)JSHTMLModElement::ConstructorAttrNum, DontEnum, 0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLModElementTable = { 63, JSHTMLModElementTableValues, 0 };

/* Hash table for constructor */

static const HashTableValue JSHTMLModElementConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLModElementConstructorTable = { 0, JSHTMLModElementConstructorTableValues, 0 };

class JSHTMLModElementConstructor : public DOMObject {
public:
    JSHTMLModElementConstructor(ExecState* exec)
        : DOMObject(exec->lexicalGlobalObject()->objectPrototype())
    {
        putDirect(exec->propertyNames().prototype, JSHTMLModElementPrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    JSValue* getValueProperty(ExecState*, int token) const;
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    virtual bool implementsHasInstance() const { return true; }
};

const ClassInfo JSHTMLModElementConstructor::s_info = { "HTMLModElementConstructor", 0, &JSHTMLModElementConstructorTable, 0 };

bool JSHTMLModElementConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLModElementConstructor, DOMObject>(exec, &JSHTMLModElementConstructorTable, this, propertyName, slot);
}

JSValue* JSHTMLModElementConstructor::getValueProperty(ExecState* exec, int token) const
{
    // The token is the numeric value of its associated constant
    return jsNumber(exec, token);
}

/* Hash table for prototype */

static const HashTableValue JSHTMLModElementPrototypeTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLModElementPrototypeTable = { 0, JSHTMLModElementPrototypeTableValues, 0 };

const ClassInfo JSHTMLModElementPrototype::s_info = { "HTMLModElementPrototype", 0, &JSHTMLModElementPrototypeTable, 0 };

JSObject* JSHTMLModElementPrototype::self(ExecState* exec)
{
    // Changed by Paul Pedriana (1/2009), as the static new was creating a memory leak. If this gets called a lot then we can consider making it a static pointer that's freed on shutdown.
    const Identifier prototypeIdentifier(exec, "[[JSHTMLModElement.prototype]]");
    return KJS::cacheGlobalObject<JSHTMLModElementPrototype>(exec, prototypeIdentifier);
}

const ClassInfo JSHTMLModElement::s_info = { "HTMLModElement", &JSHTMLElement::s_info, &JSHTMLModElementTable , 0 };

JSHTMLModElement::JSHTMLModElement(JSObject* prototype, HTMLModElement* impl)
    : JSHTMLElement(prototype, impl)
{
}

bool JSHTMLModElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLModElement, Base>(exec, &JSHTMLModElementTable, this, propertyName, slot);
}

JSValue* JSHTMLModElement::getValueProperty(ExecState* exec, int token) const
{
    switch (token) {
    case CiteAttrNum: {
        HTMLModElement* imp = static_cast<HTMLModElement*>(impl());
        return jsString(exec, imp->cite());
    }
    case DateTimeAttrNum: {
        HTMLModElement* imp = static_cast<HTMLModElement*>(impl());
        return jsString(exec, imp->dateTime());
    }
    case ConstructorAttrNum:
        return getConstructor(exec);
    }
    return 0;
}

void JSHTMLModElement::put(ExecState* exec, const Identifier& propertyName, JSValue* value)
{
    lookupPut<JSHTMLModElement, Base>(exec, propertyName, value, &JSHTMLModElementTable, this);
}

void JSHTMLModElement::putValueProperty(ExecState* exec, int token, JSValue* value)
{
    switch (token) {
    case CiteAttrNum: {
        HTMLModElement* imp = static_cast<HTMLModElement*>(impl());
        imp->setCite(valueToStringWithNullCheck(exec, value));
        break;
    }
    case DateTimeAttrNum: {
        HTMLModElement* imp = static_cast<HTMLModElement*>(impl());
        imp->setDateTime(valueToStringWithNullCheck(exec, value));
        break;
    }
    }
}

JSValue* JSHTMLModElement::getConstructor(ExecState* exec)
{
    // Changed by Paul Pedriana (1/2009), as the static new was creating a memory leak. If this gets called a lot then we can consider making it a static pointer that's freed on shutdown.
    const Identifier constructorIdentifier(exec, "[[HTMLModElement.constructor]]");
    return KJS::cacheGlobalObject<JSHTMLModElementConstructor>(exec, constructorIdentifier);
}


}