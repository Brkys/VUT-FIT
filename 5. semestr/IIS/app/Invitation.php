<?php

namespace App;

use Illuminate\Database\Eloquent\Model;

class Invitation extends Model
{
    /**
     * tabulka spojena s modelem
     */
    protected $table = 'PozvankyDoFamilii';
    public $timestamps = false;
}