#!/bin/bash

# generate protobuf source code
echo "Generating protobuf source code..."
PROTO_SRC=$(find src/proto -name "*.proto")
for proto_file in $PROTO_SRC; do
	echo "  $proto_file"
	protoc -I=src/proto --java_out=src/java/protorecord/src/main/java $proto_file
done

echo "Complete!"