package com.example.authentication.repository;

import com.example.authentication.builder.AccountBuilder;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface AccountBuilderRepository extends JpaRepository<AccountBuilder, Long> {
    Optional<AccountBuilder> findByUserName(String userName);
}