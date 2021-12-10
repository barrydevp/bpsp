package com.core.frame;

import java.nio.charset.StandardCharsets;
import java.util.HashMap;

public class VarHeader {
    private String key = "";
    private String value = "";

    public VarHeader() {
    }

    public VarHeader(String key, String value) {

        this.key = key;
        this.value = value;
    }

    public String getKey() {
        return key;
    }

    public String getValue() {
        return value;
    }

    public void setKey(String key) {
        this.key = key;
    }

    public void setValue(String value) {
        this.value = value;
    }

    public static HashMap<String, VarHeader> fromBytes(byte[] varHeadersBytes) {
        String varHeadersStr = new String(varHeadersBytes);

        return VarHeader.fromString(varHeadersStr);
    }

    public static HashMap<String, VarHeader> fromString(String varHeadersStr) {
        HashMap<String, VarHeader> varHeaders = new HashMap<>();

        if (varHeadersStr.length() <= 0) {
            return varHeaders;
        }

        int n = 0;
        int startKey = 0;
        int endKey = 0;
        int startValue = 0;

        for (; n < varHeadersStr.length(); n++) {
            char currentChar = varHeadersStr.charAt(n);
            // continue loop until found double quote `"` but not after backslash `\`
            if (n != 0 && (currentChar != '"' || varHeadersStr.charAt(n - 1) == '\\')) {
                continue;
            }

            if (startKey == 0) {
                // if startKey was not set so current double quote is open double quote of key string
                startKey = n + 1;
            } else if (endKey == 0) {
                // if endKey was not set and startKey is set so current double quote is close double quote of key string
                endKey = n;
            } else if (startValue == 0) {
                // if startValue was not set and all double quote for key is set
                // so current double quote is open double quote of value string
                startValue = n + 1;
            } else {
                // and the last is the close double quote of value string
                // so we complete read for one var header
                String key = varHeadersStr.substring(startKey, endKey);
                String value = varHeadersStr.substring(startValue, n);

                varHeaders.put(key, new VarHeader(key, value));

                // skip semicolon `;`
                n += 1;

                // reset all index cursor
                startKey = 0;
                endKey = 0;
                startValue = 0;
            }
        }

        if (startKey > 0) {
            // the cursor wasn't reset so we reach partial parse.
            throw new ExceptionInInitializerError("Invalid Variable Header.");
        }

        return varHeaders;
    }

    public static String toStr(HashMap<String, VarHeader> varHeaders) {
        StringBuilder varHeaderStrBuilder = new StringBuilder();

        for (VarHeader varHeader : varHeaders.values()) {
            varHeaderStrBuilder.append('"')
                    .append(varHeader.getKey())
                    .append("\"\"")
                    .append(varHeader.getValue())
                    .append("\";");
        }

        return varHeaderStrBuilder.toString();
    }

    public static byte[] toBytes(HashMap<String, VarHeader> varHeaders, int varHeaderSize) {
        byte[] varHeadersBytes = VarHeader.toStr(varHeaders).getBytes(StandardCharsets.UTF_8);

        if (varHeadersBytes.length != varHeaderSize) {
            throw new IllegalArgumentException("varHeaderSize is not equal to real size of HashMap VarHeader.");
        }

        return varHeadersBytes;
    }
}
