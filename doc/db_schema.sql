<<<<<<< HEAD
PRAGMA foreign_keys=ON;

CREATE TABLE IF NOT EXISTS "config" (
	"name" TEXT PRIMARY KEY NOT NULL,
	"value" TEXT
);

CREATE TABLE IF NOT EXISTS "routers" (
	"id" BLOB PRIMARY KEY,
	"encryption_key" BLOB NOT NULL,
	"signing_key" BLOB NOT NULL,
	"certificate" BLOB NOT NULL,
	"published" BLOB NOT NULL,
	"signature" BLOB NOT NULL
);

CREATE TABLE IF NOT EXISTS "router_addresses" (
	"router_id" BLOB NOT NULL REFERENCES routers(id) ON UPDATE CASCADE ON DELETE CASCADE,
	"index" INTEGER NOT NULL,
	"cost" INTEGER NOT NULL,
	"expiration" BLOB NOT NULL,
	"transport" TEXT NOT NULL,
	PRIMARY KEY(router_id,"index")
);

CREATE TABLE IF NOT EXISTS "router_address_options" (
=======
CREATE TABLE "config" (
  "name" TEXT PRIMARY KEY NOT NULL,
  "value" TEXT
);
;
CREATE TABLE "routers" (
  "id" BLOB PRIMARY KEY,
  "encryption_key" BLOB NOT NULL,
  "signing_key" BLOB NOT NULL,
  "certificate" BLOB NOT NULL,
  "published" BLOB NOT NULL,
  "signature" BLOB NOT NULL
);
;
CREATE TABLE "router_addresses" (
  "router_id" BLOB NOT NULL REFERENCES routers(id) ON UPDATE CASCADE ON DELETE CASCADE,
  "index" INTEGER NOT NULL,
  "cost" INTEGER NOT NULL,
  "expiration" BLOB NOT NULL,
  "transport" TEXT NOT NULL,
  PRIMARY KEY(router_id,"index")
);
;
CREATE TABLE "router_options" (
  "router_id" BLOB NOT NULL REFERENCES routers(id) ON UPDATE CASCADE ON DELETE CASCADE,
  "name" TEXT NOT NULL,
  "value" TEXT NOT NULL,
  PRIMARY KEY(router_id, name)
);
;
CREATE TABLE "profiles" (
    "router_id" BLOB NOT NULL REFERENCES routers(id) ON UPDATE CASCADE ON DELETE CASCADE,
    "last_seen" INTEGER,
    PRIMARY KEY("router_id")
);
;
CREATE TABLE "router_address_options" (
>>>>>>> master
	"router_id" BLOB NOT NULL,
	"index" INTEGER NOT NULL,
	"name" TEXT NOT NULL,
	"value" TEXT NOT NULL,
	PRIMARY KEY(router_id, "index", name),
<<<<<<< HEAD
	FOREIGN KEY(router_id, "index") REFERENCES router_addresses(router_id, "index")
);

CREATE TABLE IF NOT EXISTS "router_options" (
	"router_id" BLOB NOT NULL REFERENCES routers(id) ON UPDATE CASCADE ON DELETE CASCADE,
	"name" TEXT NOT NULL,
	"value" TEXT NOT NULL,
	PRIMARY KEY(router_id, name)
=======
	FOREIGN KEY(router_id, "index") REFERENCES router_addresses(router_id, "index") ON UPDATE CASCADE ON DELETE CASCADE
>>>>>>> master
);
;
