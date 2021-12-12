package com.ui;

import javax.swing.*;

public abstract class AbstractUI extends JFrame {
    static final int MAX_WIDTH = 800;
    static final int MAX_HEIGHT = 600;

    public AbstractUI(String title) {
        super(title);
    }

    public abstract void onConnected();
    public abstract void onClosed();
}
