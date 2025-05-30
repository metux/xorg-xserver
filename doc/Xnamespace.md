Xnamespace extension v1.0
=========================

This extension separates clients into several namespaces (a bit similar to
Linux's kernel namespaces), which are isolated from each other. For example,
namespaces have their own selections and clients cannot directly interact
(send messages) or access other client's resources across namespace borders.

An exception is the `root` namespace, which completely is unrestricted.

Configuration
-------------

Namespaces are defined in a separate configuration file, which is loaded at
server startup (no dynamic provisioning in this version yet). The extension
is enabled when a namespace config is passed to the Xserver via the
`-namespace <fn>` flag.


See `Xext/namespace/ns.conf.example` for a configuration file example.

Authentication / Namespace assignment
-------------------------------------

Assignment of clients into namespaces is done by the authentication token the
client is using to authenticate. Thus, token authentication needs to enabled.
