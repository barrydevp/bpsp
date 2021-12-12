package com.resources;

public class BPSPException extends Exception {
    public BPSPException() {
        super();
    }

    public BPSPException(String message) {
        super(message);
    }

    public BPSPException(String message, Throwable cause) {
        super(message, cause);
    }

    public BPSPException(Throwable cause) {
        super(cause);
    }

    protected BPSPException(String message, Throwable cause,
                            boolean enableSuppression,
                            boolean writableStackTrace) {
        super(message, cause, enableSuppression, writableStackTrace);
    }
}
