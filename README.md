# CS118 Project 1

This is the repo for winter22 cs118 project 1.

## Makefile

This provides a couple make targets for things.
By default (all target), it makes the `server` executables.

It provides a `clean` target, and `tarball` target to create the submission file as well.

## Academic Integrity Note

You are encouraged to host your code in private repositories on [GitHub](https://github.com/), [GitLab](https://gitlab.com), or other places.  At the same time, you are PROHIBITED to make your code for the class project public during the class or any time after the class.  If you do so, you will be violating academic honestly policy that you have signed, as well as the student code of conduct and be subject to serious sanctions.

## Provided Files

`server.cpp` is the entry points for the server part of the project.

## Testing

You can test your HTTP server directly using a browser, and/or a utility like `telnet` or `nc`. Your code will be graded using a script similar to the provided `test.sh`, and you should ensure that your code passes the provided tests. The final grading will use additional hidden tests to make sure your code follows the specification.

The output of `test.sh` would indicate which tests passed and failed along with the total number of passing tests. You can use this information to debug your code and make sure all the tests pass.

```
Checking HTTP status code ... pass
Checking content length ... pass
Checking if content is correct ... pass
Checking case insensitivity
Checking HTTP status code ... pass
Checking if content is correct ... pass
Checking GET without extension
Checking HTTP status code ... pass

Passed 6 tests, 0 failed
```

## TODO
Joshua Mares
005154394
joshuamares180@gmail.com

My server opens a socket to listen on, then one to communicate with.  When it recieves a request it dumps it onto the console and parses it.
It first grabs the file name and searches the directory for the closest file as the request is case insensitive.  I implemented this by 
looking only at the lowercase name of the request in the server, ignoring the file extension.  If I found the file I then grabbed the
original file name and used open and stat to grab the info requested as well as the info needed to build a proper header.  I then sent the
header and file seperately.  If the file was too large, I would send data in 4kb packets until the read stream was empty.  If I could not
find the file, I would instead send my 404.html file instead and set the header fields appropriately.  After sending the data, I closed the
communication socket and began listening for a new connection.  If an interrupt signal was passed into the program it would close both the
communication and listening socket.  Most of my problems arose from being unfamiliar with the general protocal for http headers.  I also hadnt
programmed using sockets in a while so that was an issue as well.  Both of these issues were solved with the help of the TAs as well as their
discussion videos.  One issue that I did solve but still dont understand was that for non-text based files the final \r\n from the http header
section somehow was passed into the the first byte of the images.  I got rid of the empty \r\n line and it fized the issue but Im still not sure
how it occurred.  Much of my code was heavily influenced by the design provided by the TAs.  In particular, Xinyu Ma's playsocket program was
very useful for relearning socket programming.  Also, Tianyuan Yu's header design was veyr useful for learning about the various header fields
and how to build the header.