package main.frame;

import java.nio.ByteBuffer;

import resources.Constants;

public class FrameFixedHeader {
	//**constants */
	public static final int VARS_SIZE_OFFSET = 0;
	public static final int OPCODE_OFFSET = 2;
	public static final int FLAG_OFFSET = 3;
	public static final int DATA_SIZE_OFFSET = 4;
	
	//**attributes */
	private short varsHeaderSize;
	private byte opcode;
	private byte flag;
	private int dataSize;

	//**constructors */
	public FrameFixedHeader(short varsHeaderSize, byte opcode, byte flag, int dataSize) throws Exception {
		this.varsHeaderSize = varsHeaderSize;
		this.opcode = opcode;
		this.flag = flag;
		this.dataSize = dataSize;
	}
	public FrameFixedHeader(byte[] frameHeaderBytes) throws Exception {
		if (frameHeaderBytes.length != Constants.FIXED_HEADER_SIZE) {
			throw new Exception("size of frameHeaderBytes is invalid");
		}

		ByteBuffer buf = ByteBuffer.wrap(frameHeaderBytes); // wrap to bytebuffer for easier processing
		this.varsHeaderSize = buf.getShort();
		this.opcode = buf.get();
		this.flag = buf.get();
		this.dataSize = buf.getInt();

		if (buf.hasRemaining()) {
			throw new Exception("error while initing frame fixed header");
		}
	}

	//**get & set methods */
	public short getVarsHeaderSize() {
		return varsHeaderSize;
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
	public void setVarsHeaderSize(short varsHeaderSize) {
		this.varsHeaderSize = varsHeaderSize;
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

	//**transform methods */
	public byte[] toByteArray() {
		ByteBuffer buffer = ByteBuffer.allocate(Constants.FIXED_HEADER_SIZE);
		buffer.putShort(varsHeaderSize);
		buffer.put(opcode);
		buffer.put(flag);
		buffer.putInt(dataSize);
		return buffer.array();
	}

	public void print() {
		System.out.println("(" + (int)varsHeaderSize + ", " + (int)opcode + ", " + (int)flag + ")");
	}
}
