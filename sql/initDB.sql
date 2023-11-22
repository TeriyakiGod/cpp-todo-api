CREATE TABLE
    IF NOT EXISTS Users (
        UserID TEXT PRIMARY KEY,
        Name TEXT,
        Email TEXT,
        Password TEXT
    );

CREATE TABLE
    IF NOT EXISTS Todos (
        TodoID TEXT PRIMARY KEY,
        UserID TEXT,
        Title TEXT,
        Description TEXT,
        Status TEXT
    );
