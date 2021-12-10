package com.core.client;

import com.core.frame.Frame;
import com.core.frame.VarHeader;

public abstract class Handler {
    protected boolean running;
    protected BPSPClient client;

    public Handler(BPSPClient client) {
        this.client = client;
    }

    public BPSPClient getClient() {
        return client;
    }

    public boolean isRunning() {
        return running;
    }

    public void replyOk(Frame reply) {
    }

    public void replyError(Frame reply) {
    }

    public boolean isOfFrame(Frame frame) {
        return false;
    }
}
