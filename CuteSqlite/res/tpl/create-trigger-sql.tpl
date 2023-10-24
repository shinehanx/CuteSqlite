
CREATE TRIGGER IF NOT EXISTS {<!--trigger_name-->} 
	BEFORE/AFTER  INSERT/UPDATE/DELETE
	ON "<Table Name>"
	FOR EACH ROW 
BEGIN
	/*<update-stmt>;*/
	/*<insert-stmt>;*/
	/*<delete-stmt>;*/
	/*<select-stmt>;*/
END;
