
all: awaitorder tmpfile _suping

awaitorder: awaitorder.c
	gcc -g -o awaitorder awaitorder.c


tmpfile: tmpfile.c
	gcc -g -o tmpfile tmpfile.c

_suping: _suping.c
	gcc -g -o _suping _suping.c
	@echo "*** Don't forget to   chown root _suping; chmod u+sx _suping   ***"

clean:
	rm -f tmpfile
	rm -f awaitorder
	rm -f _suping
	rm -f *~
