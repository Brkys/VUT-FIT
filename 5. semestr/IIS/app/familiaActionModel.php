<?php

namespace App;

use Illuminate\Database\Eloquent\Model;

class familiaActionModel extends Model
{
    /**
     * tabulka spojena s modelem
     */
    protected $table = 'CinnostFamilii';
    protected $primaryKey = 'id';
    public $timestamps = false;
}