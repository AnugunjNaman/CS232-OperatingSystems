To run this part do the following

(AVOID DEADLOCK)
SO IT WILL CONTINUE IN INFINITE LOOP YOU CAN PROBABLY GO ON LOL!
BETTER TO SHUT IT AFTER SOME TIME

result0.txt contains inserts and deletes in queue 0
result0.txt contains inserts and deletes in queue 1

matrix.txt helps in printing deadlock cycle

1. Install xterm (mini bash)
	sudo apt install xterm
2. After that compile all of them
3. Run ./manager <probability> <sema_key_id> <queue0_id> <queue1_id> <mutex_id> <empty_queue_count_id> <full_queue_count_id>

This id should work random except for 0 and 1 maybe few other which may interfere with semget commond used with IPC_CREAT.

I used following ./manager <any_choice_of_probabilit> 1236 332 443 2237 457 985

NOTE: TO SEE RESULTS STOP USING CTRL-C AND CHECK THEM SINCE IT WON'T STOP DUE TO DEADLOCK AVOIDANCE



(Range of probability 0.2 -> 0.89)
