using Godot;
using System;

public class TestDialogueJob : Node {

    public override void _Ready() {
    }

    public String CheckApples() {
        return "less"; // for test
    }

    public String GetApplesCount() {
        return "3 apples"; // for test
    }

    public void PrintTest() {
        GD.Print("rua"); // for test
    }
}
