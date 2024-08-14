#!/usr/bin/env -S godot -s
extends SceneTree

func _init():
	var output_file_path = "res://output.txt"
	var failed_tests = []

	var file = FileAccess.open(output_file_path, FileAccess.READ)
	var content = file.get_as_text()

	file.close()
	var lines = content.split("\n")
	for line in lines:
		if line.contains("- Failed"):
			failed_tests.append(line)

	# There is probably a better way to do this but this is the only way I found to make godot return -1/non-0
	var dir = DirAccess.open("res://")
	dir.remove(output_file_path)
	if failed_tests.size() > 0:
		OS.crash("-1")
	else:
		quit()
