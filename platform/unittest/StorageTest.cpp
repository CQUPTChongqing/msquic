/*++

    Copyright (c) Microsoft Corporation.
    Licensed under the MIT License.

--*/


#include "quic_platform.h"
#include "quic_storage.h"

#define LOG_ONLY_FAILURES
#define INLINE_TEST_METHOD_MARKUP
#include <wextestclass.h>
#include <logcontroller.h>

#include "quic_trace.h"
#include "storagetest.tmh"

using namespace WEX::Common;

#define VERIFY_QUIC_SUCCESS(result, ...) VERIFY_ARE_EQUAL(QUIC_STATUS_SUCCESS, result, __VA_ARGS__)

struct StorageTest : public WEX::TestClass<StorageTest>
{
    BEGIN_TEST_CLASS(StorageTest)
    END_TEST_CLASS()

    void ResetMsQuicRegistry()
    {
        RegDeleteTreeA(
            HKEY_LOCAL_MACHINE,
            "System\\CurrentControlSet\\Services\\MsQuic\\Parameters\\Storage\\TEST");
    }

    TEST_CLASS_SETUP(Setup)
    {
        ResetMsQuicRegistry();
        return TRUE;
    }

    TEST_CLASS_CLEANUP(Cleanup)
    {
        ResetMsQuicRegistry();
        return true;
    }

    TEST_METHOD_CLEANUP(MethodCleanup)
    {
        ResetMsQuicRegistry();
        return true;
    }

    TEST_METHOD(FailOpenNonExisting)
    {
        PQUIC_STORAGE Storage;
        VERIFY_ARE_NOT_EQUAL(
            QUIC_STATUS_SUCCESS,
            QuicStorageOpen(
                "TEST",
                QUIC_STORAGE_OPEN_FLAG_OPEN_EXISTING,
                &Storage));
    }

    TEST_METHOD(PersistKey)
    {
        PQUIC_STORAGE Storage;
        VERIFY_QUIC_SUCCESS(
            QuicStorageOpen(
                "TEST",
                QUIC_STORAGE_OPEN_FLAG_CREATE,
                &Storage));
        QuicStorageClose(Storage);
        Storage = nullptr;

        VERIFY_QUIC_SUCCESS(
            QuicStorageOpen(
                "TEST",
                QUIC_STORAGE_OPEN_FLAG_OPEN_EXISTING,
                &Storage));
        QuicStorageClose(Storage);
    }

    TEST_METHOD(PersistValue)
    {
        PQUIC_STORAGE Storage;
        VERIFY_QUIC_SUCCESS(
            QuicStorageOpen(
                "TEST",
                QUIC_STORAGE_OPEN_FLAG_CREATE,
                &Storage));
        UINT8 Value[256];
        VERIFY_QUIC_SUCCESS(
            QuicStorageWriteValue(
                Storage,
                "NAME",
                Value,
                sizeof(Value)));
        QuicStorageClose(Storage);
        Storage = nullptr;

        VERIFY_QUIC_SUCCESS(
            QuicStorageOpen(
                "TEST",
                QUIC_STORAGE_OPEN_FLAG_OPEN_EXISTING,
                &Storage));
        UINT8* PersistedValue;
        UINT32 PersistedValueLength = 0;
        VERIFY_QUIC_SUCCESS(
            QuicStorageReadValue(
                Storage,
                "NAME",
                nullptr,
                &PersistedValueLength));
        VERIFY_ARE_EQUAL(PersistedValueLength, (UINT32)sizeof(Value));
        PersistedValue = new UINT8[PersistedValueLength];
        VERIFY_IS_NOT_NULL(PersistedValue);
        VERIFY_QUIC_SUCCESS(
            QuicStorageReadValue(
                Storage,
                "NAME",
                PersistedValue,
                &PersistedValueLength));
        QuicStorageClose(Storage);
    }
};