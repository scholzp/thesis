## [Target Prediction for indirect branches](../sources/indirect_branch-cache.pdf)

* indirect jump used for example in function pointers
* Branch Target Buffer stores fallthrough and last taken address for each branch
* taken address is the last computed address for the branch
* branch address and history are hashed to form th index into the target cache