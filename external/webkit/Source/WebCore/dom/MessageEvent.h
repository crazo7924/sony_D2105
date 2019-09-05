/*
 * Copyright (C) 2007 Henry Mason (hmason@mac.com)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 */

#ifndef MessageEvent_h
#define MessageEvent_h

#include "Blob.h"    //SOMC KK PATCH Patches_w15.5
#include "DOMWindow.h"
#include "Event.h"
#include "MessagePort.h"
#include "ScriptValue.h"    //SOMC KK PATCH Patches_w15.5
#include "SerializedScriptValue.h"
#include "ArrayBuffer.h"    //SOMC KK PATCH Patches_w15.5

namespace WebCore {

    class DOMWindow;

    class MessageEvent : public Event {
    public:
        static PassRefPtr<MessageEvent> create()
        {
            return adoptRef(new MessageEvent);
        }
        //SOMC KK PATCH Patches_w15.5_begin
        static PassRefPtr<MessageEvent> create(PassOwnPtr<MessagePortArray> ports, const ScriptValue& data = ScriptValue(), const String& origin = "", const String& lastEventId = "", PassRefPtr<DOMWindow> source = 0)
        {
            return adoptRef(new MessageEvent(data, origin, lastEventId, source, ports));
        }
        //SOMC KK PATCH Patches_w15.5_end
    static PassRefPtr<MessageEvent> create(PassOwnPtr<MessagePortArray> ports, PassRefPtr<SerializedScriptValue> data, const String& origin = "", const String& lastEventId = "", PassRefPtr<DOMWindow> source = 0)    //SOMC KK PATCH Patches_w15.5
        {
            return adoptRef(new MessageEvent(data, origin, lastEventId, source, ports));
        }
    //SOMC KK PATCH Patches_w15.5_begin
    static PassRefPtr<MessageEvent> create(const String& data)
    {
        return adoptRef(new MessageEvent(data));
    }
    static PassRefPtr<MessageEvent> create(PassRefPtr<Blob> data)
    {
        return adoptRef(new MessageEvent(data));
    }
    static PassRefPtr<MessageEvent> create(PassRefPtr<ArrayBuffer> data)
    {
        return adoptRef(new MessageEvent(data));
    }
    //SOMC KK PATCH Patches_w15.5_end
        virtual ~MessageEvent();

    void initMessageEvent(const AtomicString& type, bool canBubble, bool cancelable, const ScriptValue& data, const String& origin, const String& lastEventId, DOMWindow* source, PassOwnPtr<MessagePortArray>);    //SOMC KK PATCH Patches_w15.5
        void initMessageEvent(const AtomicString& type, bool canBubble, bool cancelable, PassRefPtr<SerializedScriptValue> data, const String& origin, const String& lastEventId, DOMWindow* source, PassOwnPtr<MessagePortArray>);

        //SerializedScriptValue* data() const { return m_data.get(); }    //SOMC KK PATCH Patches_w15.5
        const String& origin() const { return m_origin; }
        const String& lastEventId() const { return m_lastEventId; }
        DOMWindow* source() const { return m_source.get(); }
        MessagePortArray* ports() const { return m_ports.get(); }

    //SOMC KK PATCH Patches_w15.5_begin
    // FIXME: Remove this when we have custom ObjC binding support.
    SerializedScriptValue* data() const;
    //SOMC KK PATCH Patches_w15.5_end
        // FIXME: remove this when we update the ObjC bindings (bug #28774).
        MessagePort* messagePort();
        // FIXME: remove this when we update the ObjC bindings (bug #28774).
        void initMessageEvent(const AtomicString& type, bool canBubble, bool cancelable, PassRefPtr<SerializedScriptValue> data, const String& origin, const String& lastEventId, DOMWindow* source, MessagePort*);

        virtual bool isMessageEvent() const;

    //SOMC KK PATCH Patches_w15.5_begin
    enum DataType {
        DataTypeScriptValue,
        DataTypeSerializedScriptValue,
        DataTypeString,
        DataTypeBlob,
        DataTypeArrayBuffer
    };
    DataType dataType() const { return m_dataType; }
    ScriptValue dataAsScriptValue() const { ASSERT(m_dataType == DataTypeScriptValue); return m_dataAsScriptValue; }
    SerializedScriptValue* dataAsSerializedScriptValue() const { ASSERT(m_dataType == DataTypeSerializedScriptValue); return m_dataAsSerializedScriptValue.get(); }
    String dataAsString() const { ASSERT(m_dataType == DataTypeString); return m_dataAsString; }
    Blob* dataAsBlob() const { ASSERT(m_dataType == DataTypeBlob); return m_dataAsBlob.get(); }
    ArrayBuffer* dataAsArrayBuffer() const { ASSERT(m_dataType == DataTypeArrayBuffer); return m_dataAsArrayBuffer.get(); }
    //SOMC KK PATCH Patches_w15.5_end

    private:
        MessageEvent();
    MessageEvent(const ScriptValue& data, const String& origin, const String& lastEventId, PassRefPtr<DOMWindow> source, PassOwnPtr<MessagePortArray>);    //SOMC KK PATCH Patches_w15.5
        MessageEvent(PassRefPtr<SerializedScriptValue> data, const String& origin, const String& lastEventId, PassRefPtr<DOMWindow> source, PassOwnPtr<MessagePortArray>);

    //SOMC KK PATCH Patches_w15.5_begin
    explicit MessageEvent(const String& data);
    explicit MessageEvent(PassRefPtr<Blob> data);
    explicit MessageEvent(PassRefPtr<ArrayBuffer> data);

    DataType m_dataType;
    ScriptValue m_dataAsScriptValue;
    RefPtr<SerializedScriptValue> m_dataAsSerializedScriptValue;
    String m_dataAsString;
    RefPtr<Blob> m_dataAsBlob;
    RefPtr<ArrayBuffer> m_dataAsArrayBuffer;
    //SOMC KK PATCH Patches_w15.5_end
        String m_origin;
        String m_lastEventId;
        RefPtr<DOMWindow> m_source;
        OwnPtr<MessagePortArray> m_ports;
    };

} // namespace WebCore

#endif // MessageEvent_h
