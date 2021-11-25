package lib;

import java.nio.ByteBuffer;

public class Frame {
	private short varsHeaderSize;
	private byte opcode;
	private byte flag;
	private int dataSize;
	private String varHeaders;
	private String data;

	//**constructors */
	public Frame() {}
	public Frame(short varsHeaderSize, byte opcode, byte flag, int dataSize, String varHeaders, String data) throws Exception {
		if (varsHeaderSize != (short)varHeaders.length()) {
			throw new ExceptionInInitializerError("size of variable header is not correct");
		}
		if (dataSize != data.length()) {
			throw new ExceptionInInitializerError("size of data is not correct");
		}
		this.varsHeaderSize = varsHeaderSize;
		this.opcode = opcode;
		this.flag = flag;
		this.dataSize = dataSize;
		this.varHeaders = varHeaders;
		this.data = data;
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
	public String getVarHeaders() {
		return varHeaders;
	}
	public String getData() {
		return data;
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
	public void setVarHeaders(String varHeaders) {
		this.varHeaders = varHeaders;
	}
	public void setData(String data) {
		this.data = data;
	}
	public void setFrameControl(byte opcode,byte flag) {
		this.opcode = opcode;
		this.flag = flag;
	}

	public byte[] toByteArray() {
		ByteBuffer buffer = ByteBuffer.allocate(Constants.FIXED_HEADER_SIZE + varsHeaderSize + dataSize);

		buffer.putShort(varsHeaderSize);
		buffer.put(opcode);
		buffer.put(flag);
		buffer.putInt(dataSize);
		buffer.put(varHeaders.getBytes());
		buffer.put(data.getBytes());

		return buffer.array();
	}
}
