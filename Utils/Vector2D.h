#pragma once
#include <iostream>
#include <cmath>

/*
 * ============================================================================
 * Vector2D - Classe utilitaire pour les vecteurs 2D
 * ============================================================================
 * Fournit toutes les opérations mathématiques courantes sur les vecteurs 2D
 * Utilisé pour les positions, vélocités, directions, etc.
 *
 * Usage:
 *   Vector2D pos(10.0f, 20.0f);
 *   Vector2D vel(1.0f, 0.5f);
 *   pos += vel * deltaTime;
 * ============================================================================
 */

class Vector2D {
public:
    float x;
    float y;

    // ========================================================================
    // CONSTRUCTEURS
    // ========================================================================

    /*
     * Constructeur par défaut: initialise à (0, 0)
     */
    Vector2D() : x(0.0f), y(0.0f) {}

    /*
     * Constructeur avec valeurs
     */
    Vector2D(float x, float y) : x(x), y(y) {}

    // ========================================================================
    // OPéRATIONS DE BASE
    // ========================================================================

    /*
     * Addition de vecteurs
     * Modifie le vecteur actuel et retourne une référence
     */
    Vector2D& Add(const Vector2D& vec) {
        this->x += vec.x;
        this->y += vec.y;
        return *this;
    }

    /*
     * Soustraction de vecteurs
     */
    Vector2D& Subtract(const Vector2D& vec) {
        this->x -= vec.x;
        this->y -= vec.y;
        return *this;
    }

    /*
     * Multiplication composante par composante
     */
    Vector2D& Multiply(const Vector2D& vec) {
        this->x *= vec.x;
        this->y *= vec.y;
        return *this;
    }

    /*
     * Division composante par composante
     */
    Vector2D& Divide(const Vector2D& vec) {
        this->x /= vec.x;
        this->y /= vec.y;
        return *this;
    }

    // ========================================================================
    // OPERATEURS ARITHMETIQUES (BINAIRES)
    // ========================================================================

    /*
     * Ces opérateurs permettent d'écrire: vec1 + vec2, vec1 - vec2, etc.
     * Ils créent un nouveau vecteur (pas de modification en place)
     */

    friend Vector2D operator+(Vector2D v1, const Vector2D& v2) {
        return v1.Add(v2);
    }

    friend Vector2D operator-(Vector2D v1, const Vector2D& v2) {
        return v1.Subtract(v2);
    }

    friend Vector2D operator*(Vector2D v1, const Vector2D& v2) {
        return v1.Multiply(v2);
    }

    friend Vector2D operator/(Vector2D v1, const Vector2D& v2) {
        return v1.Divide(v2);
    }

    // ========================================================================
    // OPERATEURS D'ASSIGNATION COMPOSEE
    // ========================================================================

    /*
     * Ces opérateurs permettent d'écrire: vec1 += vec2, vec1 -= vec2, etc.
     * Ils modifient le vecteur actuel
     */

    Vector2D& operator+=(const Vector2D& vec) {
        return this->Add(vec);
    }

    Vector2D& operator-=(const Vector2D& vec) {
        return this->Subtract(vec);
    }

    Vector2D& operator*=(const Vector2D& vec) {
        return this->Multiply(vec);
    }

    Vector2D& operator/=(const Vector2D& vec) {
        return this->Divide(vec);
    }

    // ========================================================================
    // OPERATEURS AVEC SCALAIRES
    // ========================================================================

    /*
     * Multiplication par un scalaire (entier)
     * Exemple: velocity * deltaTime
     */
    Vector2D operator*(const int& scalar) const {
        return Vector2D(this->x * scalar, this->y * scalar);
    }

    /*
     * Multiplication par un scalaire (float)
     */
    Vector2D operator*(const float& scalar) const {
        return Vector2D(this->x * scalar, this->y * scalar);
    }

    /*
     * Division par un scalaire (float)
     */
    Vector2D operator/(const float& scalar) const {
        return Vector2D(this->x / scalar, this->y / scalar);
    }

    // ========================================================================
    // OPERATIONS UTILITAIRES
    // ========================================================================

    /*
     * Remet le vecteur à zéro (0, 0)
     */
    Vector2D& zero() {
        this->x = 0;
        this->y = 0;
        return *this;
    }

    /*
     * Calcule la magnitude (longueur) du vecteur
     */
    float Magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    /*
     * Calcule la magnitude au carré (plus rapide, évite le sqrt)
     * Utile pour comparer des distances sans avoir besoin de la vraie magnitude
     */
    float MagnitudeSquared() const {
        return x * x + y * y;
    }

    /*
     * Normalise le vecteur (le rend de longueur 1)
     * Utile pour les directions
     */
    Vector2D& Normalize() {
        float mag = Magnitude();
        if (mag > 0) {
            x /= mag;
            y /= mag;
        }
        return *this;
    }

    /*
     * Retourne une copie normalisée sans modifier le vecteur original
     */
    Vector2D Normalized() const {
        Vector2D result(*this);
        return result.Normalize();
    }

    /*
     * Produit scalaire (dot product)
     * Utile pour calculer des angles, projections, etc.
     */
    float Dot(const Vector2D& vec) const {
        return x * vec.x + y * vec.y;
    }

    /*
     * Calcule la distance entre deux vecteurs (points)
     */
    float Distance(const Vector2D& vec) const {
        float dx = x - vec.x;
        float dy = y - vec.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    /*
     * Calcule la distance au carré (plus rapide)
     */
    float DistanceSquared(const Vector2D& vec) const {
        float dx = x - vec.x;
        float dy = y - vec.y;
        return dx * dx + dy * dy;
    }

    /*
     * Limite la magnitude du vecteur à une valeur maximale
     * Utile pour limiter les vitesses
     */
    Vector2D& Limit(float max) {
        float magSq = MagnitudeSquared();
        if (magSq > max * max) {
            Normalize();
            x *= max;
            y *= max;
        }
        return *this;
    }

    // ========================================================================
    // OPERATEURS DE COMPARAISON
    // ========================================================================

    bool operator==(const Vector2D& vec) const {
        return (x == vec.x && y == vec.y);
    }

    bool operator!=(const Vector2D& vec) const {
        return !(*this == vec);
    }

    // ========================================================================
    // AFFICHAGE
    // ========================================================================

    /*
     * Permet d'utiliser std::cout << vec pour afficher le vecteur
     * Format: (x, y)
     */
    friend std::ostream& operator<<(std::ostream& stream, const Vector2D& vec) {
        stream << "(" << vec.x << ", " << vec.y << ")";
        return stream;
    }

    // ========================================================================
    // VECTEURS STATIQUES UTILES
    // ========================================================================

    static Vector2D Zero() { return Vector2D(0, 0); }
    static Vector2D One() { return Vector2D(1, 1); }
    static Vector2D Up() { return Vector2D(0, -1); }      // Y n�gatif car SDL
    static Vector2D Down() { return Vector2D(0, 1); }
    static Vector2D Left() { return Vector2D(-1, 0); }
    static Vector2D Right() { return Vector2D(1, 0); }
};