CREATE TABLE
    IF NOT EXISTS Todos (
        todo_id TEXT PRIMARY KEY,
        user_id TEXT,
        title TEXT,
        description TEXT,
        status INTEGER
    );