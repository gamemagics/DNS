using Godot;
using System;

public class TestButton : Button
{
    public override void _Ready() {
        
    }
	public void OnButtonPressed() {
        this.Hide();
        var dialogueBox = GetNode<CanvasLayer>("/root/DialogueBox");
        dialogueBox.Call("StartDialogue", "res://Dialogues/test.dd");
    }
}
