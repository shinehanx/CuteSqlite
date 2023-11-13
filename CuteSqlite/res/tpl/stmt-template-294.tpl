CREATE TRIGGER IF NOT EXISTS <trigger>
	BEFORE/AFTER  INSERT/UPDATE/DELETE
	ON "<table>"
	FOR EACH ROW 
BEGIN
	<update_stmt> ;
	<insert_stmt> ;
	<delete_stmt> ;
	<select_stmt> ;
END;
