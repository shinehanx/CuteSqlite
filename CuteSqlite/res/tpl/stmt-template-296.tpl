WITH RECURSIVE
  <cte_table> ( <cte_columns> ) AS 
	( VALUES( <values> ) UNION ALL 
		SELECT <columns> FROM <table> WHERE <where> ) 
SELECT <cte_columns> FROM <cte_table> ;