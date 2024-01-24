## Subscribe transform for v2ray
This is a simple program that can transform the subscribe for the v2ray proxy.
## Usage:
``` bash
$ ./update_node.sh v2ray-subscrib-links # Updating node infor and transforming v2ray's subscirb links to json format for v2ray
$ make 
$ ./subtrans -p source.json # will print the proxy list
$ ./subtrans -s [order_index] source.json target.json # will product a config for v2ray
```

	
	
