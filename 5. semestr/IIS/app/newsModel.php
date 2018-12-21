<?php

namespace App;

use Illuminate\Database\Eloquent\Model;

class newsModel extends Model
{
    /**
     * tabulka spojena s modelem
     */
    protected $table = 'news';
    protected $primaryKey = 'date';
    public $incrementing = false;
    protected $keyType = 'string';
    public $timestamps = false;
}
