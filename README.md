## Keypad decoder sketch

This sketch is mentioned in my [answer on Arduino Stack Exchange](https://arduino.stackexchange.com/a/49888/10794).

It shows how you can interpet a keypad matrix, generating "key down" and "key up" output for each keypress.

The columns should be connected to +5V via a 10k pullup resistor, and the rows should have diodes in series with them (eg. 1N4148) to prevent "ghosting".

Example wiring for 4x4 matrix:

![Keyboard matrix](Keyboard_Matrix1.png)

Output is via the serial port in the format:

```
1nnnnnnn  -> key nnnnnnn has gone down
0nnnnnnn  -> key nnnnnnn has gone up
```

In addition, there is a "heartbeat" every couple of seconds of the value 0xFF which indicates that all keys are up. This is designed to handle the situation where the receiver might miss a key-up, and thus for the rest of the session think that (say) the Ctrl key is down.

If you set DEBUGGING to true you get human-readable output, like this:

```
No keys pressed.
Key 1 down.
Key 6 down.
Key 10 down.
Key 10 up.
Key 1 up.
Key 6 up.
No keys pressed.
No keys pressed.
```

For converting the key codes to actual keyboard keys I suggest running in debugging mode and then pressing each key on the keyboard in turn. Make a note of which key code corresponds to which key. You could then put that into an array which converts the raw "scan codes" into a character (eg. code 17 might be 'A').

---

If you are using an 87-key keyboard then you may run short of ports on the Arduino. If you make 8 columns and 11 rows that gives you 88 keys (8 x 11). The 8 inputs (the columns) could be fed into a 74HC165 input shift register thus saving a few ports. The code would need to be modified a bit to handle that, of course.
