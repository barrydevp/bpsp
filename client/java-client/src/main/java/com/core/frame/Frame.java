package com.core.frame;

import com.resources.Constants;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.time.Clock;
import java.time.Instant;
import java.util.HashMap;

class VarHeader {

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

public class Frame {
    //**constants */
    public static final int VARS_SIZE_OFFSET = 0;
    public static final int OPCODE_OFFSET = 2;
    public static final int FLAG_OFFSET = 3;
    public static final int DATA_SIZE_OFFSET = 4;
    public static final byte DEFAULT_FLAG = 0b00000000;

    // Fixed Header
    private int varHeaderSize = 0;
    private byte opcode;
    private byte flag = DEFAULT_FLAG;
    private int dataSize = 0;

    // Variable Header
    private HashMap<String, VarHeader> varHeaders = new HashMap<>();

    // Payload
    private byte[] data = new byte[0];

    // attribute
    boolean isCompleted = false;

    //**constructors */
    public Frame() {
    }

    public Frame(byte opcode) {
        this(opcode, DEFAULT_FLAG, "", "");
    }

    public Frame(FrameFixedHeader fixedHeader, String varHeaders, String data) {
        this(fixedHeader.getVarsHeaderSize(), fixedHeader.getOpcode(), fixedHeader.getFlag(), VarHeader.fromString(varHeaders), data.getBytes());
    }

    public Frame(byte opcode, String varHeadersStr, String data) {
        this(varHeadersStr.length(), opcode, DEFAULT_FLAG, VarHeader.fromString(varHeadersStr), data.getBytes());
    }

    public Frame(byte opcode, byte flag, String varHeadersStr, String data) {
        this(varHeadersStr.length(), opcode, flag, VarHeader.fromString(varHeadersStr), data.getBytes());
    }

    public Frame(byte opcode, byte flag, byte[] varHeadersBytes, byte[] data) {
        this(varHeadersBytes.length, opcode, flag, VarHeader.fromBytes(varHeadersBytes), data);
    }

    public Frame(int varHeaderSize, byte opcode, byte flag, HashMap<String, VarHeader> varHeaders, byte[] data) {
        this(varHeaderSize, opcode, flag, data.length, varHeaders, data);
    }

    public Frame(int varHeaderSize, byte opcode, byte flag, int dataSize, HashMap<String, VarHeader> varHeaders, byte[] data) {
        this.varHeaderSize = varHeaderSize;
        this.opcode = opcode;
        this.flag = flag;
        if (varHeaders != null) {
            this.varHeaders = varHeaders;
        }
        this.dataSize = dataSize;
        this.data = data;
    }

    //**get & set methods */
    public int getVarHeaderSize() {
        return varHeaderSize;
    }

    public byte getOpcode() {
        return opcode;
    }

    public byte getFlag() {
        return flag;
    }

    public int getDataSize() {
        return dataSize;
    }

    public void setOpcode(byte opcode) {
        this.opcode = opcode;
    }

    public void setFlag(byte flag) {
        this.flag = flag;
    }

    public void setDataSize(int dataSize) {
        this.dataSize = dataSize;
    }

    public HashMap<String, VarHeader> getVarHeaders() {
        return this.varHeaders;
    }

    public byte[] getData() {
        return this.data;
    }

    public int size() {
        return Constants.FIXED_HEADER_SIZE + this.varHeaderSize + this.dataSize;
    }

    public void setVarHeader(String key, String value) {
        VarHeader varHeader = new VarHeader(key, value);
        this.setVarHeader(varHeader);
    }

    public void setVarHeader(VarHeader varHeader) {
        this.varHeaders.putIfAbsent(varHeader.getKey(), varHeader);
        this.varHeaderSize += varHeader.getKey().length() + varHeader.getValue().length() + 5;
    }

    public VarHeader getVarHeader(String key) {
        return this.varHeaders.get(key);
    }

    public int putData(String data) {
        return this.putData(data.getBytes(), false);
    }

    public int putData(String data, boolean append) {
        return this.putData(data.getBytes(), append);
    }

    public int putData(byte[] data) {
        return this.putData(data, false);
    }

    public int putData(byte[] data, boolean append) {
        if (!append) {
            this.data = data;
            this.dataSize = data.length;
        } else {
            byte[] newData = new byte[data.length + this.data.length];
            System.arraycopy(this.data, 0, newData, 0, this.data.length);
            System.arraycopy(data, 0, newData, this.data.length, data.length);

            this.dataSize = newData.length;
        }

        return this.dataSize;
    }

    public void setFrameControl(byte opcode, byte flag) {
        this.opcode = opcode;
        this.flag = flag;
    }

    public boolean isCompleted() {
        return this.isCompleted;
    }

    public void build() {
        this.setVarHeader("x-time", Instant.now(Clock.systemUTC()).toString());
        this.isCompleted = true;
    }

    //**behavior methods */
    public void print() { // print detail of frame to console
        System.out.println("---------[FRAME]---------");
        // fixed header
        System.out.println("(" + this.varHeaderSize + ", " + (int) opcode + ", " + (int) flag + ")");
        // variable header
        if (this.varHeaderSize > 0) {
            System.out.println(VarHeader.toStr(this.varHeaders));
        }
        // payload
        if (this.dataSize > 0) {
            System.out.println("##########");
            System.out.println(new String(data));
        }
    }

    //**transform methods */
    public byte[] toByteArray() {
        int frameSize = this.size();

        ByteBuffer buffer = ByteBuffer.allocate(frameSize);

        buffer.putShort((short) this.varHeaderSize);
        buffer.put(this.opcode);
        buffer.put(flag);
        buffer.putInt(dataSize);
        buffer.put(VarHeader.toBytes(this.varHeaders, this.varHeaderSize));
        buffer.put(data);

        return buffer.array();
    }

    static public Frame readFrom(DataInputStream is) throws IOException {
        // fixed header
        int varHeaderSize = is.readUnsignedShort();
        byte opcode = is.readByte();
        byte flag = is.readByte();
        int dataSize = is.readInt();

        // variable header
        byte[] varHeadersBytes = new byte[varHeaderSize];
        is.readFully(varHeadersBytes);

        // payload
        byte[] data = new byte[dataSize];
        if (dataSize > 0) {
            is.readFully(data);
        }

        Frame frame = new Frame(opcode, flag, varHeadersBytes, data);
        frame.build();

        return frame;
    }

    public void sendTo(DataOutputStream os) throws IOException, IllegalArgumentException {
        if (!this.isCompleted) {
            throw new IllegalArgumentException("Frame is not completed, please call build() first.");
        }

        // fixed header
        os.writeShort(this.varHeaderSize);
        os.writeByte(this.opcode);
        os.writeByte(this.flag);
        os.writeInt(this.dataSize);

        // variable header
        if (this.varHeaderSize > 0) {
            os.write(VarHeader.toBytes(this.varHeaders, this.varHeaderSize));
        }

        // payload
        if (this.dataSize > 0) {
            os.write(this.data);
        }

        os.flush();
    }
}
