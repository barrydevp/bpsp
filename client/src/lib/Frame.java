package lib;

import java.nio.ByteBuffer;

public class Frame {
	FrameFixedHeader fixedHeader; // fixed header attribute
	//**data attributes */
	private String varHeaders;
	private String data;

	//**constants */
	

	//**constructors */
	public Frame() {}
	public Frame(FrameFixedHeader fixedHeader, String varHeaders, String data) throws Exception {
		if (fixedHeader.getVarsHeaderSize() != varHeaders.length() || fixedHeader.getDataSize() != data.length()) {
			throw new Exception("var header size or data size is not valid");
		}
		this.fixedHeader = fixedHeader;
		this.varHeaders = varHeaders;
		this.data = data;
	}
	public Frame(byte opcode, String varHeaders, String data) {
		try {
			this.fixedHeader = new FrameFixedHeader((short)varHeaders.length(),opcode,(byte)0,data.length());
		} catch (Exception e) {
			System.out.println(e);
		}
		this.varHeaders = varHeaders;
		this.data = data;
	}
	public Frame(byte opcode, byte flag, String varHeaders, String data) {
		try {
			this.fixedHeader = new FrameFixedHeader((short)varHeaders.length(),opcode,flag,data.length());
		} catch (Exception e) {
			System.out.println(e);
		}
		this.varHeaders = varHeaders;
		this.data = data;
	}
	public Frame(short varsHeaderSize, byte opcode, byte flag, int dataSize, String varHeaders, String data) throws Exception {
		if (varsHeaderSize != (short)varHeaders.length()) {
			throw new ExceptionInInitializerError("size of variable header is not correct");
		}
		if (dataSize != data.length()) {
			throw new ExceptionInInitializerError("size of data is not correct");
		}
		this.fixedHeader = new FrameFixedHeader((short)varHeaders.length(),opcode,flag,data.length());
		this.varHeaders = varHeaders;
		this.data = data;
	}

	//**get & set methods */
	public String getVarHeaders() {
		return varHeaders;
	}
	public String getData() {
		return data;
	}
	public void setVarHeaders(String varHeaders) {
		this.varHeaders = varHeaders;
		this.fixedHeader.setVarsHeaderSize((short)varHeaders.length());
	}
	public void setData(String data) {
		this.data = data;
		this.fixedHeader.setDataSize(data.length());
	}
	public void setFrameControl(byte opcode,byte flag) {
		this.fixedHeader.setOpcode(opcode);
		this.fixedHeader.setFlag(flag);
	}
	public void setFixedHeader(FrameFixedHeader fixedHeader) {
		this.fixedHeader = fixedHeader;
	}

	public byte[] toByteArray() {
		ByteBuffer buffer = ByteBuffer.allocate(Constants.FIXED_HEADER_SIZE + fixedHeader.getVarsHeaderSize() + fixedHeader.getDataSize());
		buffer.put(fixedHeader.toByteArray());
		buffer.put(varHeaders.getBytes());
		buffer.put(data.getBytes());
		return buffer.array();
	}
}
