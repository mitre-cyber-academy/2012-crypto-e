Crypto Challenge -- 500 points.

Summary: Two files are available to the solvers: FlagMe contains the text "FlagMe", and
RSA_pubkey.pem provide an RSA public key.  An html introduction point tells the students
that they need to submit an RSA signature of "FlagMe" under the given RSA public key to
server 1 to obtain the flag, and that server 2 issues signatures.  Server 2 issues 
"plain" RSA signatures of whatever you submit to it, but it will refuse to issue a signature
of "FlagMe."  

Challenge files:

	index.html -- The challenge entry point.
	/files -- this should be a readable sub-folder.
      /files/FlagMe -- pointed to by index.html
      /files/RSA_pubkey.pem -- pointed to by index.html

Building:

	Server2 should run "signer500 RSA_modulus.txt RSA_d.txt input" where input is its input.
	Server1 should run "verifier500 RSA_modulus.txt RSA_e.txt input" where input is its input.

	signer500.c and verifier500.c are provided; they need to be compiled as follows:

	gcc -std=gnu99 -I/usr/local/include -Wall -c -o signer500.o signer500.c
	gcc -std=gnu99 -Wall -L/usr/local/lib -o signer500 signer500.o -lgmp

	gcc -std=gnu99 -I/usr/local/include -Wall -c -o verifier500.o verifier500.c
	gcc -std=gnu99 -Wall -L/usr/local/lib -o verifier500 verifier500.o -lgmp


Solution walkthrough:

	First, students will want to experiment with Server 2 to understand how it works.
If they submit "FlagMe" they get a response that can be recognized as an error (it's text).
If they submit other short strings, they will get various responses.  If they submit a string
that's too long, they will get an error response stating that the message must be "less than
the RSA modulus."  

      Some experimentation may be needed to understand how input strings are converted to numbers:
the way it actually works is that the input file is taken to be a number in base 256, with
the most significant digit being the first byte, the second being the second most, and so on.
Students will also need to learn about .pem files, at least enough to know that OpenSSL can 
manipulate them.  

      You can enter the shell command:

      "openssl rsa -text -pubout -pubin -in RSA_pubkey.pem -out RSA_pubkey.txt"

      to produce the RSA public key in a clear format, as shown in RSA_pubkey.txt.

      This can be edited to produce N.txt and e.txt which are hex (in string) format 
representations of the RSA modulus and the public exponent.  

	The following experiment verifies how the signatures are working.  The RSA signature
on the number 1 is always 1, since it is 1 raised to the power of the private exponent.  So
if the students can figure out how to give the number 1 as an input, they should observe
the number 1 as an output from the signature server.  "1_1b" is a 1-byte file specifying 
the number 1.  Run Server 2 on 1_1b to see how the answer 1 looks in our number format.
Run Server 2 on 1_4b to see that 1_4b is the 4-byte representation of the number 1, which
confirms we are using a big endian byte order.

	The solution is to use a "blind signature."  The student needs to compute f^d mod N,
where f is the value of "FlagMe", and has a server that will compute x^d mod N for any x other
than f^d.  So we want to compute f*2^e mod N, where e is the public exponent.  The signing server
will then sign to compute f^d*2^{ed} mod N.  However, 2^{ed} mod N = 2 mod N, so this is just
f^d * 2 mod N.  We can divide by 2 modulo N to obtain f^d.

	blind500.c is code to "blind" an input binary value and output the blinded value as a hex
string.  To build:

	gcc -std=gnu99 -I/usr/local/include -Wall -c -o blind500.o blind500.c
	gcc -std=gnu99 -Wall -L/usr/local/lib -o blind500 blind500.o -lgmp

	I have provided RSA_modulus.txt and RSA_e.txt.  Run:

	blind500 RSA_modulus.txt RSA_e.txt FlagMe

	The result is in the file "FlagMe.blinded.txt".
	Copy and paste the hex string in a hex editor into a new 128-byte file and save the file.
	The result of this is the file "FlagMe.blinded.binary".  

	Run Server 2 on FlagMe.blinded.binary to obtain a signature as a hex string; this is in
the file "FlagMe.blinded.signature".

	unblind500.c is code to "unblind" an input hex string, basically, it just divides the hex
string by 2 modulo N.  To build:

	gcc -std=gnu99 -I/usr/local/include -Wall -c -o unblind500.o unblind500.c
	gcc -std=gnu99 -Wall -L/usr/local/lib -o unblind500 unblind500.o -lgmp

	unblind500 RSA_modulus.txt FlagMe.blinded.signature

	The output is in the file FlagMe.my.signature.

	Now run Server 1 on FlagMe.my.signature and the flag should be given as output.  
