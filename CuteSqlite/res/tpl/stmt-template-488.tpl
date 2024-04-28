ALTER TABLE <table>
	RENAME TO <new_table_name>,
	ADD COLUMN <column> NOT NULL DEFAULT ('') ,
	RENAME COLUMN <column> TO <new_column> ,
	DROP COLUMN <column> ;

	