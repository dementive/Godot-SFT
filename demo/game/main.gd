extends Summator

# Must be _enter_tree.
# _ready would NOT work hear because of the object lifetime and how SFT runs the tests.
func _enter_tree() -> void:
	add(10)
	add(20)
	add(30)
	print(get_total())
	reset()

	# Run SFT tests.
	test()
