package com.resources;

public enum Flag {
    ACK("ACK", (byte) 0b10000000),
    PUB_ECHO("PUB_ECHO", (byte) 0b01000000),
    ;

    private final String text;
    private final byte value;

    Flag(final String text, final byte value) {
        this.text = text;
        this.value = value;
    }

    public String getText() {
        return text;
    }

    public byte getValue() {
        return value;
    }
}
