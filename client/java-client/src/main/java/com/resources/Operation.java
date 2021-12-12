package com.resources;

public enum Operation {
	INFO("INFO", (byte)1),
	CONNECT("CONNECT", (byte)2),
	PUB("PUB",(byte)3),
	SUB("SUB",(byte)4),
	UNSUB("UNSUB",(byte)5),
	MSG("MSG",(byte)6),
	OK("OK",(byte)7),
	ERR("ERR",(byte)8)
	;

	private final String text;
	private final byte code;

	Operation(final String text, final byte code) {
		this.text = text;
		this.code = code;
	}

	public String getText() {
			return text;
	}

	public byte getCode() {
		return code;
	}
}
