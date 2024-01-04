UPDATE Todos
SET
    title = ?,
    description = ?,
    status = ?
WHERE
    todo_id = ?;