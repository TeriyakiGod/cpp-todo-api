CREATE TABLE
    IF NOT EXISTS Users (
        user_id TEXT PRIMARY KEY,
        name TEXT,
        email TEXT,
        password TEXT,
        role INT
    );