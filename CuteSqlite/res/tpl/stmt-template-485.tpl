CREATE TABLE IF NOT EXISTS <table> (
	id INTEGER NOT NULL DEFAULT 0,
	<column> TEXT NOT NULL DEFAULT '',
	<column> BLOB NULL,
	<column> REAL NOT NULL DEFAULT 0.0,
	<column> NUMERIC NOT NULL DEFAULT 0.0,
	CONSTAINT <constraint_name> PRIMARY KEY (id AUTOINCREMENT),
	CONSTAINT <constraint_name> UNIQUE (<columns>)
);