CREATE TABLE
    IF NOT EXISTS Todos (
        TodoID TEXT PRIMARY KEY,
        UserID TEXT,
        Title TEXT,
        Description TEXT,
        Status TEXT
    );